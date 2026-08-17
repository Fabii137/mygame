#pragma once

void assertFuncProduction(const char *expression, const char *file_name,
                          unsigned const line_number,
                          const char *comment = "---");

void assertFuncInternal(const char *expression, const char *file_name,
                        unsigned const line_number,
                        const char *comment = "---");

#if DEVELOPMENT_BUILD == 1

#define permaAssert(expression)                                                \
  (void)((!!(expression)) ||                                                   \
         (assertFuncInternal(#expression, __FILE__, (unsigned)(__LINE__)), 0))

#define permaAssertComment(expression, comment)                                \
  (void)((!!(expression)) ||                                                   \
         (assertFuncInternal(#expression, __FILE__, (unsigned)(__LINE__),      \
                             comment),                                         \
          1))

#else

#define permaAssert(expression)                                                \
  (void)((!!(expression)) ||                                                   \
         (assertFuncProduction(#expression, __FILE__, (unsigned)(__LINE__)),   \
          0))

#define permaAssertComment(expression, comment)                                \
  (void)((!!(expression)) ||                                                   \
         (assertFuncProduction(#expression, __FILE__, (unsigned)(__LINE__),    \
                               comment),                                       \
          1))

#endif

#if DEVELOPMENT_BUILD == 1
#define permaAssertDevelopment permaAssert
#define permaAssertCommentDevelopment permaAssertComment

#else
#define permaAssertDevelopment
#define permaAssertCommentDevelopment
#endif
