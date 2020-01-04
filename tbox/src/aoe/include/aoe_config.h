
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef AOE_CONFIG_H
#define AOE_CONFIG_H


#define AOE_REFACTORING 1

#define AOE_VERSION "AOE_20200101_V0.1"


//for rt-thread finsh
#define RT_USING_FINSH
#define RT_USING_POSIX
//#define FINSH_USING_SYMTAB
//#define FINSH_USING_DESCRIPTION
#define FINSH_USING_MSH
#define FINSH_USING_HISTORY
#define FINSH_USING_MSH_DEFAULT
#define FINSH_HISTORY_LINES 5

#define FINSH_ARG_MAX 10


#endif
#ifdef __cplusplus
}
#endif
