/*
 * string_helper.h
 *
 *  Created on: 2016年2月23日
 *      Author: chenzhuo
 */

#ifndef NETIO_INCLUDE_STRING_HELPER_H_
#define NETIO_INCLUDE_STRING_HELPER_H_


inline void SplitString(const std::string& sSrc, std::string::value_type cSep, std::vector<std::string>& vecString)
{
    vecString.clear();

    if(sSrc.empty()) return;

    std::string sTmp(sSrc);

    while(!sTmp.empty())
    {
        string::size_type nPos = sTmp.find(cSep);
        if(nPos == std::string::npos)
        {
        	break;
        }

        vecString.push_back(sTmp.substr(0, nPos));
        sTmp.erase(0, nPos + 1);
    }

    vecString.push_back(sTmp);  // add final element
}

inline std::string Trim(const std::string &s)
{
	if (s.length() == 0)
	{
		return s;
	}
	size_t iBeg = s.find_first_not_of(" \f\n\r\t\v");
	size_t iEnd = s.find_last_not_of(" \f\n\r\t\v");
	if (iBeg == string::npos||iEnd==string::npos||iEnd<iBeg)
	{
		return "";
	}

	return string(s, iBeg, iEnd - iBeg + 1);
}

/**
 * 出现重复key的参数，结合stl map insert的特性，将以第一个添加进去的值为准。
 */
inline void strPairAppendToMap(const std::string & sSrc,std::map<std::string,std::string> & mpStr)
{
	vector<string> vStr;
	SplitString(sSrc,'&',vStr);
	for(vector<string>::const_iterator p = vStr.begin(); p != vStr.end(); ++p)
	{
		vector<string> vTmp;
		SplitString(*p,'=',vTmp);
		if(vTmp.size() == 2)
		{
			string sKey = Trim(vTmp[0]);
			string sVal = Trim(vTmp[1]);
			mpStr.insert(map<string,string>::value_type(sKey,sVal));
		}
	}
	return;
}


#endif /* NETIO_INCLUDE_STRING_HELPER_H_ */
