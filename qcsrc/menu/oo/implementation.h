#ifndef INTERFACE
#define INTERFACE
#endif

#ifndef IMPLEMENTATION
#define IMPLEMENTATION
#endif

#ifdef CLASS
#undef CLASS
#undef EXTENDS
#undef METHOD
#undef ATTRIB
#undef ATTRIBARRAY
#undef ENDCLASS
#endif

#define SUPER

#define CLASS(cname)
#define EXTENDS(base)                      \
                                           #undef SUPER\
                                           #define SUPER base
#define METHOD(cname,name,prototype)
#define ATTRIB(cname,name,type,val)
#define ATTRIBARRAY(cname,name,type,cnt)
#define ENDCLASS(cname)
