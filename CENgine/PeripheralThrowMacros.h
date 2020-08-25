#pragma once

// Error exception helper macro
#define PRPH_EXCEPT( hr ) Peripheral::HrException( __LINE__,__FILE__,(hr) )
#define PRPH_LAST_EXCEPT() Peripheral::HrException( __LINE__,__FILE__,GetLastError() )
#define PRPH_INVALID_INDEX_EXCEPT(hr) Peripheral::InvalidPeripheralIndexException(__LINE__, __FILE__, (hr))
#define PRPH_UNSUPPORTED_TYPE_EXCEPT(hr) Peripheral::UnsupportedPeripheralTypeException(__LINE__, __FILE__, (hr))