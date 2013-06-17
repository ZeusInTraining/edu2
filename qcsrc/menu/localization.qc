// localization.qc
// Andreas Kirsch 2008

#define STR_MAP_BEGIN string( string original ) _ = { switch( original ) {
#define STR_MAP(org,mapped) case org : return mapped;
#define STR_MAP_END default: return original; } };

#include "str_map.inc"

//STR_MAP_BEGIN
//STR_MAP_END

#undef STR_MAP_END
#undef STR_MAP
#undef STR_MAP_BEGIN

