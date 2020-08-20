//RTC_Export.h

#ifndef __AR_RTC_EXPORT_H__
#define __AR_RTC_EXPORT_H__

#ifndef AR_EXPORT
# if defined(WIN32)
#  if defined(RTC_BUILD) && defined(DLL_EXPORT)
#   define AR_EXPORT __declspec(dllexport)
#  else
#   define AR_EXPORT
#  endif
# elif defined(__GNUC__) && defined(RTC_BUILD)
#  define AR_EXPORT __attribute__ ((visibility ("default")))
# else
#  define AR_EXPORT
# endif
#endif	// AR_EXPORT

#endif	// __AR_RTC_EXPORT_H__