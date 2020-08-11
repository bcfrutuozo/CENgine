#pragma once

// Error exception helper macro
#define WND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define WND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define WND_NOGFX_EXCEPT() Window::NoGraphicsException( __LINE__,__FILE__)