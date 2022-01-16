#pragma once

#include <codecvt>

namespace std {

#if defined(UNICODE) || defined(_UNICODE)
using tstring = wstring;
#else
using tstring = string;
#endif

wstring string_to_wstring(const string& str)
{
    using convert_type = codecvt_utf8<wchar_t>;
    wstring_convert<convert_type, wchar_t> cv;
    return cv.from_bytes(str);
}

string wstring_to_string(const wstring& wstr)
{
    using convert_type = codecvt_utf8<wchar_t>;
    wstring_convert<convert_type, wchar_t> cv;
    return cv.to_bytes(wstr);
}

wstring to_wstring(const string& str)
{
    return string_to_wstring(str);
}

wstring to_wstring(const wstring& wstr)
{
    return wstr;
}

string to_string(const wstring& wstr)
{
    return wstring_to_string(wstr);
}

string to_string(const string& str)
{
    return str;
}

}
