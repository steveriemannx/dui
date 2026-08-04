# libpag Open Source Project License Constraints Overview (main project + third-party components)
## I. libpag Main Project License Constraints (Apache License Version 2.0)
| Constraint category | Core requirements |
|----------|----------|
| Copyright and modification notices | 1. When modifying main project code, add a prominent notice to the modified files (noting the modifier, the content, and the date);<br>2. When distributing derivative works in source form, retain all original copyright, patent, trademark, and attribution notices (unrelated notices may be omitted). |
| Redistribution file requirements | 1. Whether distributed in source or binary form, provide recipients with a complete copy of the Apache 2.0 license (in a `LICENSE` file or by providing the official link: [http://www.apache.org/licenses/](http://www.apache.org/licenses/));<br>2. If the main project contains a `NOTICE` file, derivative works must include valid attribution notices in at least one of: the accompanying `NOTICE` file, the source code, or the runtime interface. |
| Patent and litigation restrictions | 1. Receive a free, worldwide, perpetual, non-exclusive patent license (covering only patents necessarily infringed by the contributor's code alone or combined with the main project);<br>2. If you initiate patent litigation claiming the main project/contributions infringe, all patent licenses terminate immediately. |
| Trademark usage | You may not use the trade names, trademarks, service marks, or product names of Tencent or the contributors, except for "reasonable descriptive use" (e.g., "developed based on Tencent's open source libpag"). |
| Disclaimer and liability | 1. The main project is provided "AS IS", without any express or implied warranties (e.g., merchantability, fitness for a particular purpose);<br>2. Unless required by law (e.g., willful misconduct or gross negligence), no liability for indirect damages (loss of goodwill, business interruption, etc.). |


## II. Third-Party Component License Constraints
| Third-party component | License type | Core constraints (of particular concern for development) |
|------------|----------|----------------------------|
| libavc | Apache 2.0 | Same constraints as the libpag main project (retain copyright notices, include a copy of the license with distribution), and must state "this component has been modified by Tencent" |
| lz4 (lib directory) | BSD 2-Clause | 1. Source distribution must retain the original copyright notice, license terms, and disclaimer;<br>2. Binary distribution must reproduce the above notices in the installation documentation |
| lz4 (non-lib directories) | GPL v2 | 1. Copyleft: derivative works based on modifications of this code must be released as a whole under GPL v2; cannot be closed-source for commercial use;<br>2. Binary distribution must provide complete source code (or a written offer for source code obtainable within 3 years) |
| tgfx | BSD 3-Clause | In addition to BSD 2-Clause: you may not use the names of Tencent or tgfx contributors to endorse/promote your software (e.g., "certified by Tencent" is a violation) |
| ffavc | LGPL v2.1 | 1. If your software is a closed-source application, you may link the ffavc library, but you must provide the modified source code of ffavc (if you modified it);<br>2. Prominently state in the software that "the ffavc library is used, licensed under LGPL v2.1" |
| vendor_tools/rttr | MIT License | 1. Free for commercial use and modification, but all copies of the software must retain the original copyright notice and the MIT license notice;<br>2. You may add your own copyright notice, but may not remove or modify the original notice |
| QT family (e.g., QT Quick, QT Widgets) | LGPL v3 | 1. If QT components are combined with your closed-source software, you must provide the modified source code of the QT components and the "installation information" (so users can relink your software after modifying QT);<br>2. The runtime "About" interface must include the QT copyright notice and a reference to the LGPL v3 license |


## III. Cross-License General Compliance Recommendations
1. **Avoiding license conflicts**: if you use lz4 (non-lib directories, GPL v2), be aware of GPL "copyleft" — the entire software (including your own code) must be released under GPL v2; if closed-source commercial use is required, it is recommended to use only lz4 (lib directory, BSD 2-Clause).
2. **Notice file management**: in the software's `LICENSE` directory, store each component's license text separately (e.g., `Apache-2.0.txt`, `GPL-v2.txt`), clearly distinguishing each component's license, avoiding merging or trimming notices.
3. **Contribution guidelines**: when submitting code to official libpag, contributions are by default subject to Apache 2.0 (unless a separate license agreement is signed with Tencent); confirm before submitting that the code has no third-party rights disputes.
4. **Closed-source key checks**: in closed-source development, make sure all used components support closed-source linking (e.g., Apache 2.0, BSD, LGPL family), and avoid GPL components; if LGPL components are used, provide the modified source code and installation information as required.
