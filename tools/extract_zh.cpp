/** extract_zh - extract Chinese UI strings from XML resources and C++ sources
 *
 *  Usage (run from the dui repo root; root defaults to the current dir):
 *    extract_zh <extract|report|apply> [map_file]
 *    extract_zh                       # no args = 'extract'
 *    extract_zh extract [tools/zh_map.json]
 *    extract_zh report  [tools/zh_map.json]
 *    extract_zh apply   [tools/zh_map.json]
 *
 *  C++ port of the former tools/extract_zh.py (no Python needed).
 *
 *  Extraction targets (strings shown to users / literals):
 *    - XML:  any attribute value containing CJK (text, tooltip_text, prompttext, ...)
 *    - XML:  element text content containing CJK (RichText etc.)
 *    - C++:  DUI_T("...") / L"..." literals containing CJK
 *  Excluded (kept as-is):
 *    - bin/resources/lang/zh_CN.txt (language pack)
 *    - MultiLang/MultiLang.xml and global.xml text_id mechanism is runtime-switched
 *
 *  Scanned dirs (under ROOT, mirroring extract_zh.py):
 *    XML:  bin/resources/themes/default
 *    CODE: src, examples, cmake, build
 *
 *  Build:
 *    clang++ -std=c++17 -O2 tools/extract_zh.cpp -o extract_zh
 */

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// ---------- UTF-8 / CJK helpers ----------

// U+4E00..U+9FFF ("一".."鿿", i.e. the CJK Unified Ideographs used in the
// Python original) are 3-byte UTF-8 sequences with a leading byte 0xE4..0xE9.
static bool isCjk(const std::string& s, size_t i) {
    if (i + 3 > s.size()) return false;
    const unsigned char c = static_cast<unsigned char>(s[i]);
    return c >= 0xE4 && c <= 0xE9;
}

static bool containsCjk(const std::string& s) {
    for (size_t i = 0; i + 3 <= s.size(); ++i) {
        if (isCjk(s, i)) return true;
    }
    return false;
}

// ASCII name char ([a-zA-Z0-9_], enough for XML attribute names)
static bool isNameChar(unsigned char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') || c == '_';
}

// ---------- JSON (minimal parser/writer for zh_map.json: {"k": "v", ...}) ----------

static bool jsonParseString(const std::string& s, size_t& i, std::string& out) {
    if (i >= s.size() || s[i] != '"') return false;
    ++i;
    out.clear();
    while (i < s.size()) {
        const char c = s[i];
        if (c == '"') { ++i; return true; }
        if (c == '\\') {
            ++i;
            if (i >= s.size()) return false;
            char e = s[i++];
            switch (e) {
                case '"':  out.push_back('"');  break;
                case '\\': out.push_back('\\'); break;
                case '/':  out.push_back('/');  break;
                case 'n':  out.push_back('\n'); break;
                case 't':  out.push_back('\t'); break;
                case 'r':  out.push_back('\r'); break;
                case 'b':  out.push_back('\b'); break;
                case 'f':  out.push_back('\f'); break;
                case 'u': {
                    if (i + 4 > s.size()) return false;
                    unsigned hex = 0;
                    for (int k = 0; k < 4; ++k) {
                        char h = s[i + k];
                        hex <<= 4;
                        if (h >= '0' && h <= '9') hex |= (unsigned)(h - '0');
                        else if (h >= 'a' && h <= 'f') hex |= (unsigned)(h - 'a' + 10);
                        else if (h >= 'A' && h <= 'F') hex |= (unsigned)(h - 'A' + 10);
                        else return false;
                    }
                    i += 4;
                    if (hex < 0x80) {
                        out.push_back((char)hex);
                    } else if (hex < 0x800) {
                        out.push_back((char)(0xC0 | (hex >> 6)));
                        out.push_back((char)(0x80 | (hex & 0x3F)));
                    } else {
                        out.push_back((char)(0xE0 | (hex >> 12)));
                        out.push_back((char)(0x80 | ((hex >> 6) & 0x3F)));
                        out.push_back((char)(0x80 | (hex & 0x3F)));
                    }
                    break;
                }
                default: return false;  // not produced by json.dump(ensure_ascii=False)
            }
        } else {
            out.push_back(c);
            ++i;
        }
    }
    return false;
}

static bool loadMap(const std::string& path, std::map<std::string, std::string>& m) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    std::string s((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (s.empty()) return false;
    size_t i = s.find('{');
    if (i == std::string::npos) return false;
    ++i;
    while (i < s.size()) {
        while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')) ++i;
        if (i < s.size() && s[i] == '}') return true;
        std::string key, val;
        if (!jsonParseString(s, i, key)) return false;
        while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')) ++i;
        if (i >= s.size() || s[i] != ':') return false;
        ++i;
        while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')) ++i;
        if (!jsonParseString(s, i, val)) return false;
        m[key] = val;
        while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')) ++i;
        if (i >= s.size()) return false;
        if (s[i] == ',') { ++i; continue; }
        if (s[i] == '}') return true;
        return false;
    }
    return false;
}

static std::string jsonEscape(const std::string& s) {
    std::string out;
    for (const char c : s) {
        switch (c) {
        case '\\': out += "\\\\"; break;
        case '"':  out += "\\\""; break;
        case '\n': out += "\\n";  break;
        case '\r': out += "\\r";  break;
        case '\t': out += "\\t";  break;
        case '\b': out += "\\b";  break;
        case '\f': out += "\\f";  break;
        default: {
            if (static_cast<unsigned char>(c) < 0x20) {
                char buf[8];
                snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)c);
                out += buf;
            } else {
                out.push_back(c);
            }
        }
        }
    }
    return out;
}

static bool writeMap(const std::string& path, const std::map<std::string, std::string>& m) {
    std::ofstream f(path, std::ios::binary);
    if (!f) return false;
    // json.dump(indent=1, ensure_ascii=False) format: one space indentation,
    // raw UTF-8, no trailing newline.
    f << "{\n";
    bool first = true;
    for (const auto& kv : m) {
        if (!first) f << ",\n";
        first = false;
        f << " \"" << jsonEscape(kv.first) << "\": \"" << jsonEscape(kv.second) << "\"";
    }
    f << "\n}";
    return (bool)f;
}

// ---------- File traversal (mirrors extract_zh.py skip rules) ----------

static const std::vector<std::string> kSkipSuffixes = {
    ".png", ".jpg", ".jpeg", ".gif", ".svg", ".ico", ".icns", ".bmp", ".webp",
    ".apng", ".lottie", ".json", ".rc", ".vcxproj", ".sln", ".filters", ".app"
};
static const std::vector<std::string> kSkipDirs = {
    "build_temp", ".git", "third_party", "lib", "bin/cef",
    "CefBrowser.app", "cef.app", "native backend", "skia"
};

static bool isSkipped(const fs::path& rel) {
    std::string r = "/" + rel.generic_string();
    for (const auto& d : kSkipDirs) {
        if (r.find("/" + d + "/") != std::string::npos) return true;
    }
    const std::string name = rel.filename().string();
    for (const auto& s : kSkipSuffixes) {
        if (name.size() >= s.size() && name.compare(name.size() - s.size(), s.size(), s) == 0) {
            return true;
        }
    }
    return false;
}

static bool isXmlFile(const fs::path& p) {
    const std::string name = p.filename().string();
    return name.size() >= 4 && name.compare(name.size() - 4, 4, ".xml") == 0;
}

static bool isCodeFile(const fs::path& p) {
    const std::string name = p.filename().string();
    const std::vector<std::string> suffixes = {".cpp", ".h", ".cmake", ".sh", ".txt"};
    for (const auto& s : suffixes) {
        if (name.size() >= s.size() && name.compare(name.size() - s.size(), s.size(), s) == 0) {
            return true;
        }
    }
    return false;
}

static void walk(const fs::path& dir, const fs::path& rel,
                 std::vector<fs::path>& out) {

    std::error_code ec;
    fs::directory_iterator it(dir, fs::directory_options::skip_permission_denied, ec);
    for (; it != fs::directory_iterator(); it.increment(ec)) {
        if (ec) break;
        const fs::path entryRel = rel / it->path().filename();
        const std::string name = it->path().filename().string();
        if (it->is_directory(ec)) {
            // prune at walk level like the Python os.walk dirnames filter
            if (name != "build_temp" && name != ".git") {
                walk(it->path(), entryRel, out);
            }
        } else {
            if (isSkipped(entryRel)) continue;
            if (!isXmlFile(entryRel) && !isCodeFile(entryRel)) continue;
            out.push_back(it->path());
        }
    }
}

static void collectFiles(const fs::path& root, std::vector<fs::path>& out) {
    const fs::path bases[] = {
        root / "bin/resources/themes/default",  // XML
        root / "src", root / "examples", root / "cmake", root / "build"  // CODE
    };
    for (const auto& base : bases) {
        std::error_code ec;
        if (fs::is_directory(base, ec)) walk(base, base.filename(), out);
    }
}

static bool readFile(const std::string& path, std::string& out) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return false;
    out.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    return true;
}

static bool writeFile(const std::string& path, const std::string& data) {
    std::ofstream f(path, std::ios::binary | std::ios::trunc);
    if (!f) return false;
    f.write(data.data(), (std::streamsize)data.size());
    return (bool)f;
}

// ---------- Extraction ----------

static void extractFromXml(const std::string& text, std::set<std::string>& out) {
    // attr="value-with-CJK"
    for (size_t i = 0; i + 2 < text.size(); ++i) {
        if (text[i] != '=' || text[i + 1] != '"') continue;
        size_t nameEnd = i;
        size_t nameStart = nameEnd;
        while (nameStart > 0 && isNameChar((unsigned char)text[nameStart - 1])) --nameStart;
        if (nameStart == nameEnd) continue;  // no attribute name before '='
        const size_t vStart = i + 2;
        const size_t vEnd = text.find('"', vStart);
        if (vEnd == std::string::npos) break;
        const std::string value = text.substr(vStart, vEnd - vStart);
        if (containsCjk(value)) out.insert(value);
        i = vEnd;
    }

    // element text nodes containing CJK (e.g. RichText content)
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] != '>') continue;
        const size_t tStart = i + 1;
        const size_t lt = text.find('<', tStart);
        const size_t gt = text.find('>', tStart);
        if (lt == std::string::npos) break;
        if (gt != std::string::npos && gt < lt) { i = gt; continue; }  // '>' before '<': not a text node
        std::string node = text.substr(tStart, lt - tStart);
        if (containsCjk(node)) {
            const size_t b = node.find_first_not_of(" \t\r\n");
            const size_t e = node.find_last_not_of(" \t\r\n");
            if (b != std::string::npos) out.insert(node.substr(b, e - b + 1));
        }
        i = lt;
    }
}

// One-line C string literal starting at 'open' (index of the opening '"').
// requireParen mirrors the Python regexes: DUI_T("...") needs the closing ')',
// L"..." does not. Raw newlines are not allowed inside.
static std::string codeLiteral(const std::string& text, size_t open, bool requireParen) {
    std::string content;
    for (size_t i = open + 1; i < text.size(); ++i) {
        const char ch = text[i];
        if (ch == '\n') return {};
        if (ch == '\\') {
            if (i + 1 < text.size()) {
                content += ch;
                content += text[++i];
            }
            continue;
        }
        if (ch == '"') {
            const size_t after = i + 1;
            if (!requireParen || (after < text.size() && text[after] == ')')) return content;
            return {};
        }
        content += ch;
    }
    return {};
}

static void extractFromCode(const std::string& text, std::set<std::string>& out) {
    // DUI_T("...") with CJK (single-line only)
    const std::string kOpen1 = "DUI_T(\"";
    for (size_t i = 0; (i = text.find(kOpen1, i)) != std::string::npos; ++i) {
        const std::string s = codeLiteral(text, i + kOpen1.size() - 1, true);
        if (!s.empty() && containsCjk(s)) out.insert(s);
    }
    // L"..." with CJK (single-line only)
    for (size_t i = 0; (i = text.find("L\"", i)) != std::string::npos; ++i) {
        const std::string s = codeLiteral(text, i + 1, false);
        if (!s.empty() && containsCjk(s)) out.insert(s);
    }
}

static void extractAll(const fs::path& root, std::set<std::string>& strings) {
    std::vector<fs::path> files;
    collectFiles(root, files);
    for (const auto& p : files) {
        std::string text;
        if (!readFile(p.string(), text)) continue;
        if (isXmlFile(p)) extractFromXml(text, strings);
        else extractFromCode(text, strings);
    }
}

// ---------- Apply (translations) ----------

static void replaceAll(std::string& text, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    std::string::size_type p = 0;
    while ((p = text.find(from, p)) != std::string::npos) {
        text.replace(p, from.size(), to);
        p += to.size();
    }
}

static void applyFiles(const fs::path& root, const std::map<std::string, std::string>& map) {
    // sort by length descending so longer strings are replaced first
    std::vector<std::pair<std::string, std::string>> items(map.begin(), map.end());
    std::sort(items.begin(), items.end(),
              [](const auto& a, const auto& b) { return a.first.size() > b.first.size(); });

    std::vector<fs::path> files;
    collectFiles(root, files);
    int applied = 0;
    for (const auto& p : files) {
        std::string text;
        if (!readFile(p.string(), text)) continue;
        const std::string orig = text;
        for (const auto& item : items) {
            const std::string& zh = item.first;
            const std::string& en = item.second;
            if (zh.empty() || zh == en) continue;
            if (isXmlFile(p)) {
                // attribute value: backslash -> \\, quote -> &quot;
                std::string escEn;
                for (const char c : en) {
                    if (c == '\\') escEn += "\\\\";
                    else if (c == '"') escEn += "&quot;";
                    else escEn.push_back(c);
                }
                replaceAll(text, "=\"" + zh + "\"", "=\"" + escEn + "\"");
                // element text node
                replaceAll(text, ">" + zh + "<", ">" + en + "<");
            } else {
                // C string escaping: backslash -> \\, quote -> \", newline -> \n
                std::string escEn;
                for (const char c : en) {
                    if (c == '\\') escEn += "\\\\";
                    else if (c == '"') escEn += "\\\"";
                    else if (c == '\n') escEn += "\\n";
                    else escEn.push_back(c);
                }
                replaceAll(text, "DUI_T(\"" + zh + "\")", "DUI_T(\"" + escEn + "\")");
                replaceAll(text, "L\"" + zh + "\"", "L\"" + escEn + "\"");
                replaceAll(text, "\"" + zh + "\"", "\"" + escEn + "\"");
            }
        }
        if (text != orig) {
            if (writeFile(p.string(), text)) ++applied;
            else std::cerr << "ERROR: cannot write " << p.string() << std::endl;
        }
    }
    std::cout << "Applied to " << applied << " files" << std::endl;
}

// ---------- Commands ----------

static void cmdExtract(const fs::path& root, const std::string& mapFile) {
    std::set<std::string> strings;
    extractAll(root, strings);

    // load existing map to preserve already-translated entries
    std::map<std::string, std::string> existing;
    loadMap(mapFile, existing);  // missing file -> start fresh (like Python)

    std::map<std::string, std::string> out;
    for (const auto& s : strings) out[s] = existing.count(s) ? existing.at(s) : "";
    if (!writeMap(mapFile, out)) {
        std::cerr << "ERROR: cannot write " << mapFile << std::endl;
        return;
    }
    int done = 0;
    for (const auto& kv : out) if (!kv.second.empty()) ++done;
    std::cout << "Unique strings: " << out.size() << ", translated: " << done
              << ", pending: " << out.size() - done << std::endl;
}

static void cmdReport(const std::string& mapFile) {
    std::map<std::string, std::string> mapping;
    if (!loadMap(mapFile, mapping)) {
        std::cerr << "ERROR: cannot load " << mapFile << std::endl;
        return;
    }
    int done = 0;
    for (const auto& kv : mapping) if (!kv.second.empty()) ++done;
    std::cout << "Total: " << mapping.size() << ", translated: " << done
              << ", pending: " << mapping.size() - done << std::endl;
    // sorted list of pending, print first 50
    std::vector<std::string> pending;
    for (const auto& kv : mapping) if (kv.second.empty()) pending.push_back(kv.first);
    for (size_t i = 0; i < pending.size() && i < 50; ++i) {
        std::cout << "   " << pending[i] << std::endl;
    }
}

static int cmdApply(const fs::path& root, const std::string& mapFile) {
    std::map<std::string, std::string> mapping;
    if (!loadMap(mapFile, mapping)) {
        std::cerr << "ERROR: cannot load " << mapFile << std::endl;
        return 1;
    }
    std::vector<std::string> missing;
    for (const auto& kv : mapping) if (kv.second.empty()) missing.push_back(kv.first);
    if (!missing.empty()) {
        std::cout << "ERROR: " << missing.size() << " strings not translated yet" << std::endl;
        for (size_t i = 0; i < missing.size() && i < 20; ++i) {
            std::cout << "   " << missing[i] << std::endl;
        }
        return 1;
    }
    applyFiles(root, mapping);
    return 0;
}

int main(int argc, char** argv) {
    const std::string cmd = argc > 1 ? argv[1] : "extract";
    const fs::path root = fs::current_path();
    const std::string mapFile = argc > 2 ? argv[2] : (root / "tools/zh_map.json").string();

    if (cmd == "extract") {
        cmdExtract(root, mapFile);
    } else if (cmd == "report") {
        cmdReport(mapFile);
    } else if (cmd == "apply") {
        return cmdApply(root, mapFile);
    } else {
        // print usage
        std::cout << "Usage: extract_zh <extract|report|apply> [map_file]\n"
                  << "  (no args = extract; default map: tools/zh_map.json)\n";
    }
    return 0;
}
