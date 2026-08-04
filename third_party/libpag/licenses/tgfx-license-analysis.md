# tgfx Open Source Project License Analysis
## I. tgfx Main Project License Overview
The tgfx main project is licensed under the **BSD 3-Clause License**; only third-party components follow other open source licenses. Tencent has modified parts of the software in this project (referred to as "Tencent Modifications"); all Tencent-modified content is copyrighted by Tencent (Copyright (C) 2025 Tencent).

### BSD 3-Clause License Core Terms
| Clause category | Specific requirements |
|----------|----------|
| Redistribution conditions | 1. Redistribution in source form: the original copyright notice, this list of conditions, and the following disclaimer must be retained;<br>2. Redistribution in binary form: the original copyright notice, this list of conditions, and the disclaimer must be reproduced in the accompanying documentation and/or other materials;<br>3. Trademark restriction: without specific prior written permission of the copyright holder or contributors, their names may not be used to endorse or promote products derived from this software. |
| Disclaimer | This software is provided by the copyright holders and contributors "AS IS", without any express or implied warranties, including but not limited to the implied warranties of merchantability and fitness for a particular purpose. |
| Limitation of liability | In no event shall the copyright holders or contributors be liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including but not limited to procurement of substitute goods or services, loss of data or profits, business interruption, etc.) arising in any way out of the use of this software, whether based on contract, strict liability, or tort (including negligence), even if advised of the possibility of such damage. |


## II. Third-Party Component License Details
### 1. freetype
| Item | Content |
|------|------|
| Copyright | Copyright (C) 2006-2022 by David Turner, Robert Wilhelm, and Werner Lemberg |
| License | The FreeType Project LICENSE |
| Core license terms | - **No warranty**: The FreeType Project is provided "AS IS", without any express or implied warranties, including merchantability and fitness for a particular purpose; the authors or copyright holders are not liable for damages arising from the use of or inability to use the project.<br>- **Redistribution requirements**:<br>  1. Source redistribution must retain the license file (`FTL.TXT`) unmodified; the original copyright notice must be retained in full; modified files must be clearly marked in the accompanying documentation;<br>  2. Binary redistribution must state in the distributed documentation that the software is in part based on the work of the FreeType team; adding a link to the FreeType official website to the documentation is recommended (not mandatory);<br>- **Trademark and promotion restrictions**: without prior written permission, the FreeType authors, contributors, and users may not use each other's names for commercial, advertising, or promotional purposes; it is recommended to refer to the software in documentation or advertising materials using the specific designations "FreeType Project", "FreeType Engine", etc.<br>- **Copyright ownership**: the FreeType Project is copyrighted by David Turner, Robert Wilhelm, and Werner Lemberg (1996-2000); unless otherwise stated in a file, this license applies to all original distribution files. |
| Additional notes | It is recommended to add the credit notice: "Portions of this software are copyright © <year> The FreeType Project (www.freetype.org). All rights reserved.", where `<year>` must be replaced with the year corresponding to the FreeType version actually used. |


### 2. libpng
| Item | Content |
|------|------|
| Copyright | - Versions 1.0.7 (July 1, 2000) through 1.6.33 (September 28, 2017): Copyright (c) 2000-2002, 2004, 2006-2017 Glenn Randers-Pehrson;<br>- Versions 0.97 (January 1998) through 1.0.6 (March 20, 2000): Copyright (c) 1998-2000 Glenn Randers-Pehrson;<br>- Versions 0.89 (June 1996) through 0.96 (May 1997): Copyright (c) 1996-1997 Andreas Dilger;<br>- Versions 0.5 (May 1995) through 0.88 (January 1996): Copyright (c) 1995-1996 Guy Eric Schalnat, Group 42, Inc. |
| License | libpng license |
| Core license terms | - **Disclaimer**: the PNG reference library is provided "AS IS"; the contributors and Group 42, Inc. make no express or implied warranties, including merchantability and fitness for a particular purpose, and are not liable for direct, indirect, incidental, special, exemplary, or consequential damages arising from the use of the library, even if advised of the possibility of such damage.<br>- **Use and redistribution permissions**: permission is granted to freely use, copy, modify, and distribute the source code or parts thereof, for any purpose, subject to the following restrictions:<br>  1. The origin of the source code must not be misrepresented;<br>  2. Modified versions must be clearly marked as modified and must not be passed off as the original source code;<br>  3. The copyright notice must not be removed or altered from any source or modified-version source distribution.<br>- **Commercial use encouraged**: explicitly permitted and encouraged to use this source code as a component in commercial products that support the PNG file format, at no charge; although acknowledgments are not mandatory, they are appreciated. |
| Additional notes | - Trademark: "libpng" is not registered as a trademark in any jurisdiction, but the copyright holder claims "common-law trademark protection" in jurisdictions that recognize common-law trademarks;<br>- OSI certification: libpng is OSI-certified open source software, but OSI was not involved in the disclaimer added in version 1.0.7;<br>- Export control: the copyright holder believes libpng's Export Control Classification Number (ECCN) is EAR99; it is open-source public software, contains no encryption content, and is not subject to export controls or the International Traffic in Arms Regulations (ITAR). |


### 3. Multiple components (libwebp, pathKit, skcms, highway)
| Item | Content |
|------|------|
| Components and copyright | - libwebp: Copyright (c) 2010, Google Inc. All rights reserved.<br>- pathKit: Copyright (c) 2011 Google Inc. All rights reserved.<br>- skcms: Copyright 2018 Google Inc.<br>- highway: Copyright (c) The Highway Project Authors. All rights reserved. |
| License | BSD 3-Clause License (Tencent has modified parts of the software; the modifications are copyrighted by Tencent) |
| Core license terms | Fully consistent with the BSD 3-Clause License of the tgfx main project, specifically:<br>- Source redistribution must retain the original copyright notice, license terms, and disclaimer;<br>- Binary redistribution must reproduce the above notices in the accompanying documentation/materials;<br>- Without prior written permission, the copyright holders' or contributors' names may not be used to endorse or promote products;<br>- The software is provided "AS IS", without warranty, and without liability for indirect damages. |


### 4. libjpeg-turbo
| Item | Content |
|------|------|
| Copyright | Copyright (C) 1991-1996, Thomas G. Lane; Copyright (C) 2017, D. R. Commander |
| License | IJG License (Independent JPEG Group License) and other third-party component licenses |
| Core license terms | - **Disclaimer**: the authors make no express or implied warranties, including quality, accuracy, merchantability, and fitness for a particular purpose; the software is provided "AS IS", and users bear the risk of use;<br>- **Use and redistribution permissions**: permission is granted to freely use, copy, modify, and distribute the software (or parts thereof) for any purpose, subject to the following conditions:<br>  1. If distributing source code, include this README file (containing the copyright and no-warranty statements, unmodified); modified files must be clearly marked in the accompanying documentation;<br>  2. If distributing executable code only, the accompanying documentation must state that "this software is based in part on the work of the Independent JPEG Group";<br>  3. Users bear any adverse consequences of using the software; the authors are not liable for any damages.<br>- **Trademark and promotion restrictions**: the IJG authors' or companies' names may not be used in advertising or publicity relating to this software or derivative products; the software may only be referred to as "Independent JPEG Group's software";<br>- **Commercial use support**: explicitly permitted and encouraged to use this software as the basis of commercial products; product suppliers bear all warranty or liability claims. |
| Additional notes | - The software originally contained GIF read/write code; to avoid the Unisys LZW patent dispute (now expired), GIF read support was removed, and GIF writing was simplified to produce "uncompressed GIFs" readable by all standard GIF decoders;<br>- Other third-party component licenses can be referenced at: https://github.com/libjpeg-turbo/libjpeg-turbo/blob/2.0.0/LICENSE.md |


### 5. zlib
| Item | Content |
|------|------|
| Copyright | Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler |
| License | Zlib License |
| Core license terms | - **Disclaimer**: the software is provided "AS IS", without any express or implied warranties; the authors are not liable for damages arising from the use of the software;<br>- **Use and redistribution permissions**: anyone may use the software for any purpose (including commercial applications), freely modify and redistribute it, subject to the following restrictions:<br>  1. The origin of the software must not be misrepresented; you may not claim to have written the original software; if used in a product, acknowledgment in the product documentation is appreciated (not mandatory);<br>  2. Modified source versions must be clearly marked as modified and must not be passed off as the original software;<br>  3. This notice must not be removed or altered from any source distribution. |
| Additional notes | The data format used by the zlib library is described by RFC documents (RFC 1950, RFC 1951, RFC 1952), viewable via the links: http://tools.ietf.org/html/rfc1950 (zlib format), rfc1951 (deflate format), rfc1952 (gzip format). |


### 6. harfbuzz
| Item | Content |
|------|------|
| Copyright | Copyright © 2010-2020 Google, Inc.; Copyright © 2018-2020 Ebrahim Byagowi; Copyright © 2019-2020 Facebook, Inc.; Copyright © 2012 Mozilla Foundation; Copyright © 2011 Codethink Limited; Copyright © 2008,2010 Nokia Corporation and/or its subsidiary(-ies); Copyright © 2009 Keith Stribley; Copyright © 2009 Martin Hosken and SIL International; Copyright © 2007 Chris Wilson; Copyright © 2005-2006,2020-2021 Behdad Esfahbod; Copyright © 2005 David Turner; Copyright © 2004,2007-2010 Red Hat, Inc.; Copyright © 1998-2004 David Turner and Werner Lemberg |
| License | "Old MIT" license |
| Core license terms | - **Use and redistribution permissions**: without written agreement and license fee, the software and its documentation may be used, copied, modified, and distributed for any purpose, provided that all copies of the software include the original copyright notice and the following two paragraphs:<br>  1. "IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."<br>  2. "THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS."<br>- **Disclaimer and liability**: the copyright holder provides no warranties; the software is provided "AS IS", with no obligation for maintenance, support, updates, etc., and no liability for damages arising from the use of the software. |


### 7. libexpat
| Item | Content |
|------|------|
| Copyright | Copyright (c) 1998-2000 Thai Open Source Software Center Ltd and Clark Cooper; Copyright (c) 2001-2022 Expat maintainers |
| License | MIT License |
| Core license terms | - **Use permissions**: permission is granted to obtain copies of the software and associated documentation files (the "Software") free of charge, and to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software without restriction;<br>- **Core condition**: include the original copyright notice and this permission notice in all copies or substantial portions of the Software;<br>- **Disclaimer and liability**: the Software is provided "AS IS", without any express or implied warranties, including but not limited to the implied warranties of merchantability, fitness for a particular purpose, and non-infringement; the authors or copyright holders are not liable for any claims, damages, or other liability arising from the use of or inability to use the Software, whether based on contract, tort, or other legal theory. |


### 8. swiftshader
| Item | Content |
|------|------|
| Copyright | Copyright (c) swiftshader original author and authors |
| License | Apache License Version 2.0 and other third-party component licenses |
| Core license terms (Apache 2.0) | - **Definitions**: defines the meaning of "License", "Licensor", "Legal Entity", "You", "Source", "Object", "Work", "Derivative Works", "Contribution", "Contributor", etc. (consistent with the standard Apache 2.0 license);<br>- **Copyright license**: each contributor grants a perpetual, worldwide, non-exclusive, royalty-free, irrevocable copyright license, including copying, preparing derivative works, publicly displaying, publicly performing, sublicensing, and distributing the Work and Derivative Works (in source or object form);<br>- **Patent license**: each contributor grants a perpetual, worldwide, non-exclusive, royalty-free, irrevocable (except as stated in this section) patent license, applicable only to patent claims necessarily infringed by the contributor's Contribution alone or combined with the Work; if you initiate patent litigation alleging the Work or a Contribution infringes, the relevant patent licenses terminate as of the date the litigation is filed;<br>- **Redistribution conditions**:<br>  1. Provide a copy of this license to other recipients when distributing;<br>  2. Modified files must carry prominent notices describing the changes;<br>  3. When distributing Derivative Works in source form, retain the copyright, patent, trademark, and attribution notices of the original Work (except unrelated notices);<br>  4. If the original Work contains a "NOTICE" file, Derivative Works must include its attribution notices in the specified locations (except unrelated notices);<br>- **Submitting contributions**: submissions are by default under this license, unless a separate license agreement is signed with the Licensor;<br>- **Trademark restrictions**: no permission to use the Licensor's trademarks is granted, except as necessary for describing the origin of the Work and reproducing the NOTICE file contents;<br>- **Disclaimer and liability**: the Work is provided "AS IS", without warranty; contributors are not liable for indirect damages (except as required by law or agreed in writing). |
| Additional notes | - Source code link: https://swiftshader.googlesource.com/SwiftShader;<br>- Other third-party component licenses: https://swiftshader.googlesource.com/SwiftShader/+/refs/heads/master/LICENSE.txt |


### 9. flatbuffers
| Item | Content |
|------|------|
| Copyright | Copyright (c) flatbuffers original author and authors |
| License | Apache License Version 2.0 |
| Core license terms | Fully consistent with the Apache 2.0 terms that apply to swiftshader (including copyright license, patent license, redistribution conditions, submission of contributions, trademark restrictions, disclaimer and liability, etc.); this document contains a complete copy of that license. |


### 10. angle
| Item | Content |
|------|------|
| Copyright | Copyright 2018 The ANGLE Project Authors. All rights reserved. |
| License | BSD 3-Clause License and other third-party component licenses |
| Core license terms (BSD 3-Clause) | Consistent with the BSD 3-Clause License of the tgfx main project, including:<br>- Source redistribution retains notices;<br>- Binary redistribution reproduces notices;<br>- Name usage requires prior written permission;<br>- No warranty and no liability for indirect damages. |
| Additional notes | Other third-party component licenses: https://github.com/google/angle/tree/main/third_party |


### 11. digestpp
| Item | Content |
|------|------|
| Copyright | No specific copyright holder; released into the public domain |
| License | Public domain (see https://unlicense.org) |
| Core license terms | - **Use permissions**: anyone is free to copy, modify, publish, use, compile, sell, or distribute the software (in source or compiled binary form) for any purpose, commercial or non-commercial, by any means;<br>- **Copyright waiver**: in jurisdictions that recognize copyright laws, the author dedicates all copyright interest in the software to the public domain, waiving all present and future copyright rights in the software;<br>- **Disclaimer**: the software is provided "AS IS", without any express or implied warranties, including merchantability, fitness for a particular purpose, and non-infringement; the author is not liable for any claims, damages, or other liability arising from the use of the software, whether based on contract, tort, or other legal theory. |


### 12. concurrentqueue
| Item | Content |
|------|------|
| Copyright | Copyright (c) 2013-2016, Cameron Desrochers. All rights reserved. |
| License | BSD 2-Clause License and other third-party component licenses |
| Core license terms (BSD 2-Clause) | - **Redistribution conditions**:<br>  1. Source redistribution must retain the original copyright notice, this list of conditions, and the disclaimer;<br>  2. Binary redistribution must reproduce the above notices in the accompanying documentation and/or other materials;<br>- **Disclaimer and liability**: the software is provided "AS IS", without any express or implied warranties; the copyright holders or contributors are not liable for direct, indirect, incidental, special, exemplary, or consequential damages arising from the use of the software, whether based on contract, strict liability, or tort (including negligence). |
| Additional notes | Other third-party component licenses: https://github.com/cameron314/concurrentqueue/tree/master/benchmarks |


### 13. lz4
| Item | Content |
|------|------|
| Copyright | Copyright (c) 2011-2020, Yann Collet. All rights reserved. |
| License | BSD 2-Clause License and other third-party component licenses |
| Core license terms (BSD 2-Clause) | Fully consistent with the BSD 2-Clause License terms that apply to concurrentqueue; this document contains a complete copy of that license. |
| Additional notes | Other third-party component licenses: https://github.com/lz4/lz4/tree/dev/lib |


## III. General Compliance Recommendations
1. **License inventory and matching**: before development, identify the tgfx main project and each third-party component used, and build a "component-license" mapping table to avoid missing special terms (e.g., the "copyleft" of GPL-type licenses — not involved in this project, but be alert to license conflicts when introducing new components later).
2. **Notice and documentation management**:
   - Retain all components' original copyright notices and license texts, stored by component category (e.g., create files such as `BSD-3-Clause-tgfx.txt`, `FreeType-LICENSE.txt` under the project's `LICENSE` directory);
   - When distributing the software, provide license copies and required notices as each license requires (e.g., the FreeType credit notice, libpng modification markings).
3. **Trademark and name usage**: strictly observe the name-usage restrictions in the BSD 3-Clause, FreeType, and other licenses; without prior written permission, do not use the names of Tencent, Google, the FreeType team, etc. to endorse or promote products.
4. **Modification and contribution management**:
   - When modifying components, mark the modifications as required by the license (e.g., Apache 2.0 modification notices, libpng modified-version markings);
   - When contributing to third-party components (e.g., swiftshader, angle), follow their contribution guidelines and clarify the licensing (default license or a separate agreement).
5. **Disclaimer and risk notice**: add a clear disclaimer to the software documentation or the "About" interface, informing users that the software is provided "AS IS", without warranty, and that users bear the risk of use.
