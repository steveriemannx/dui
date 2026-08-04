# libpag License Core Information Analysis
## I. Copyright Holder Information
The libpag copyright holders must be distinguished between the "core project" and "third-party components", as follows:
| Copyright category | Specific copyright holder | Scope |
|--------------|------------|----------|
| Core project (including Tencent's modifications) | Tencent | 1. The libpag main project code and the "Tencent Modifications" (Tencent's modifications to third-party components), with the copyright year stated as 2025;<br>2. Third-party components modified by Tencent (e.g., libavc, tgfx, ffavc): the modifications belong to Tencent |
| Third-party components (original copyright) | Original copyright holders of each component | 1. libavc: 2015 The Android Open Source Project;<br>2. lz4: 2011-2020 Yann Collet;<br>3. tgfx: 2023 Tencent (standalone third-party component, original copyright belongs to Tencent);<br>4. ffavc: 2021 Tencent (standalone third-party component, original copyright belongs to Tencent);<br>5. vendor_tools: Dom Chen (2021-present);<br>6. harfbuzz: Google, Ebrahim Byagowi, Facebook, Mozilla Foundation and others (1998-2021);<br>7. rttr: Axel Menzel (2014-2018);<br>8. QT family components (QT Quick, Widgets, OpenGL, Core, etc.): the original authors of each component |


## II. License File Classification (License Types)
libpag uses a **"main license + per-third-party-component licenses" mixed licensing model**: the main project is based on Apache License Version 2.0, while third-party components follow different open source licenses. The classification and terms are as follows:
### 1. Core License: Apache License Version 2.0 (applies to the main project)
This is the base license of the libpag main project and Tencent's modifications. It is a permissive license, fully consistent with the standard Apache 2.0, with the following key contents:
| Clause category | Core content |
|----------|----------|
| Copyright and patent grants | 1. Contributors (including Tencent) grant a perpetual, worldwide, non-exclusive, royalty-free copyright license covering copying, modification, distribution, sublicensing, etc.;<br>2. Contributors grant a patent license (covering only "patents necessarily infringed by the contribution alone or combined with the main project"); if the user initiates patent litigation against the main project, the patent license terminates as of the date the litigation is filed |
| Redistribution constraints | 1. Provide recipients with a copy of the Apache 2.0 license;<br>2. Modified files must carry a modification notice;<br>3. Retain original copyright, patent, trademark and other notices (notices unrelated to the derivative work may be omitted);<br>4. If the main project contains a `NOTICE` file, derivative works must include its attribution notices in documentation, source code, or the runtime interface |
| Disclaimer and liability | 1. The software is provided "AS IS", without express or implied warranties (including merchantability, fitness for a particular purpose, non-infringement);<br>2. Contributors are not liable for direct/indirect damages arising from use of the software (except where required by law) |

### 2. Third-Party Component Licenses: multiple open source licenses (per component)
Third-party components use different licenses depending on their functionality, and each must be followed according to the component type:
| License type | Representative component | Core characteristics |
|----------|----------|----------|
| Apache License Version 2.0 | libavc (including Tencent's modifications) | Same as the main project license; free modification and commercial use; retain notices and provide a copy of the license |
| BSD 2-Clause License | lz4 (`lib` directory files) | Only two core constraints: retain the copyright notice on source/binary distribution; no name-usage restrictions; supports commercial use with no copyleft |
| GNU General Public License v2 (GPLv2) | lz4 (non-`lib` directory files, e.g., `programs`/`tests`) | 1. Strong "copyleft": derivative works must be open source as a whole and licensed under GPLv2;<br>2. Complete source code must be provided; closed-source distribution is forbidden;<br>3. No commercial-use restriction, but downstream users' freedom to modify must be preserved |
| BSD 3-Clause License | tgfx | Three constraints: retain notices, reproduce notices in binary distribution, no endorsing use of the copyright holder's name; supports commercial use with no copyleft |
| GNU Lesser General Public License v2.1 (LGPLv2.1) | ffavc | 1. Weak "copyleft": only the component itself must be open source; applications linking it may stay closed-source;<br>2. Component source code must be provided; modifications may be redistributed;<br>3. Modified versions must remain under LGPLv2.1 |
| MIT License | vendor_tools, rttr | Only one core constraint: retain the copyright and license notices; free modification and commercial use with no additional conditions |
| MIT Style license | harfbuzz | Similar to MIT: core constraint is "retain copyright notice + disclaimer"; no other restrictions; permissions equivalent to MIT |
| GNU Lesser General Public License v3 (LGPLv3) | QT family components (QT Quick, Widgets, Core, etc.) | 1. Weak "copyleft": applications linking the components may stay closed-source, but modified versions of the components must be open source;<br>2. Provide the components' complete source code and installation information (e.g., instructions for recompilation after modification);<br>3. No additional restrictions on downstream users' rights |
| GNU General Public License v3 (GPLv3) | QT family components (some scenarios) | 1. Strong "copyleft": if QT components are modified and integrated into a derivative work, the derivative work as a whole must be open source under GPLv3;<br>2. Provide complete source code and modification records, ensuring downstream users can freely modify and redistribute |


### 3. Key Rules for Mixed Licensing
1. **License priority**: the main project follows Apache 2.0; third-party components follow their own licenses first; where a component contains Tencent modifications, the modified parts must comply with both the "main project license" and the "component's original license" (e.g., libavc's modified parts must satisfy both Apache 2.0 and the original component license).
2. **Copyleft risk**: when using GPLv2/GPLv3-licensed components (e.g., lz4 non-`lib` directories, some QT components), be aware of their "copyleft" — integrating these components with closed-source code may require open-sourcing the entire work; assess compliance risks in advance.
3. **Notice retention obligations**: when distributing all components, retain the copyright notices as required by each license (e.g., MIT requires the author's notice, GPL requires the full license text); they may not be trimmed or modified.
