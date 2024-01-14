#pragma once

#define EXPECT_THROW_EX(statement, exception_message) \
{ \
	try \
	{ \
		(statement); \
		FAIL(); \
	} \
	catch (std::exception& ex) \
	{ \
		EXPECT_STREQ((exception_message), ex.what()); \
	} \
} \
