#include <StringUtil.hpp>

#include <fstream>
#include <sstream>

std::vector<std::string> split(const std::string& s, char delimiter, bool skip_empty)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter)) {
      if (skip_empty && token.length() == 0)
        continue;

      tokens.push_back(token);
   }
   return tokens;
}

// https://stackoverflow.com/a/9359324/5768099
template <std::ctype_base::mask mask>
class IsNot
{
    std::locale myLocale;       // To ensure lifetime of facet...
    std::ctype<char> const* myCType;
public:
    IsNot( std::locale const& l = std::locale() )
        : myLocale( l )
        , myCType( &std::use_facet<std::ctype<char> >( l ) )
    {
    }
    bool operator()( char ch ) const
    {
        return ! myCType->is( mask, ch );
    }
};

typedef IsNot<std::ctype_base::space> IsNotSpace;

std::string trim(std::string const& input)
{
    std::string::const_iterator right = std::find_if(input.rbegin(), input.rend(), IsNotSpace() ).base();
    std::string::const_iterator left = std::find_if(input.begin(), right, IsNotSpace());
    return std::string(left, right);
}
