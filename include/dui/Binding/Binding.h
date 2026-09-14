#ifndef UI_BINDING_H_
#define UI_BINDING_H_

/** Data-binding module (umbrella header).

 This module is what makes an MVVM style possible on top of dui, but it is not
 itself tied to that pattern: it binds any ObservableObject to any control, so
 an MVC-style model class can be bound the same way. MVVM is one way to use it.

 Purely additive: nothing here changes the behavior of code that does not call
 it, and no existing library source is modified by it. The imperative style --
 FindControl + AttachClick + SetText -- keeps working exactly as before, and the
 two styles can be mixed freely in the same window.
*/

#include "dui/Binding/ObservableObject.h"
#include "dui/Binding/BindingGroup.h"

#endif // UI_BINDING_H_
