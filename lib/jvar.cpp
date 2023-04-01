#include "jvar.h"

#include <string.h>
#define  PICOJSON_USE_INT64
#include "picojson.h"
#include <cstdio>
#include <regex>
//#include "ParallelLogger.hpp"



//#define //logwarn(x) ;

std::string jvarTypesNames[7] = {"JBOOL","JNUMBER","JLONGINTEGER","JSTRING","JOBJECT","JARRAY","JNULL"};

jvar_types jvar::getType(){return type;}
char * jvar::getBuffer(){return buffer;}

jvar::~jvar()
{
    deleteCurrent();
}

//jvar iteratable constructor (type for for each)
JvarIterableType::JvarIterableType(jvar* parg) : root(parg), iters(NULL)
{
    iters=NULL;
    switch (root->getType())
    {
        case JOBJECT:
            iters = new struct JObjectIterators();
            iters->it = ((jMapType<std::string,jvar>*)root->getBuffer())->begin();
            iters->itend = ((jMapType<std::string,jvar>*)root->getBuffer())->end();
            p = &(iters->it->second);
            if (iters->it==iters->itend)
                p=NULL;
        break;
        case JARRAY:
            i=0;
            if (root->size()>0)
                p = &((*root)[(size_t)0]);
            else
                p=NULL;   
        break;
        default:
            p = parg;
        break;
    }
}
//jvar iteratable constructor (contructor for end of for each)
JvarIterableType::JvarIterableType(jvar* parg, bool end) : root(p),iters(NULL)
{
    p = NULL;
    iters=NULL;
}
JvarIterableType::~JvarIterableType()
{
    if (iters!=NULL)
    {
        delete iters;
    }
}
void JvarIterableType::operator++() 
{ 
    switch(root->getType())
    {
        case JOBJECT:
            (iters->it)++;
            if ((iters->it!=iters->itend))
                p = &(iters->it->second);
            else
                p =NULL;
        break;
        case JARRAY:
            i++;
            if (i>=root->size())
                p =NULL;
            else
                p = &((*root)[i]);
        break;
        default:
            p =NULL;
    }
}
bool JvarIterableType::operator!=(JvarIterableType rhs) 
{
    //std::cout << "addr1: " << p << "addr2: " << rhs.p << std::endl;
    return p != rhs.p;
}

jvar jvar::split(std::string separator)
{
    if (type!=JSTRING)
        return MNULL;
    std::string *mystring = ((std::string*)buffer);
    size_t pos = mystring->find(separator);
    size_t length = pos;
    size_t start = 0;
    size_t index=0;
    jvar ret = ja{};
    if (pos==std::string::npos)
    {
        ret[ZERO] = (*(std::string*)buffer);
        return ret;
    }
    while(pos!=std::string::npos)
    {
        ret[index] = mystring->substr(start,length);
        start = pos + 1 ;
        pos = mystring->find(separator,start);
        if (pos==std::string::npos && start < mystring->length())
        {
            index++;
            ret[index] = mystring->substr(start);
            break;
        }
        length = pos - start;
        index++;
        
    }
    return ret;
}
jvar jvar::join(std::string separator)
{
    if (type!=JARRAY)
        return MNULL;
    jvar ret = "";
    size_t arrSize = size();
    for (size_t i = 0; i < arrSize; i++)
    {
        ret+= (*((std::vector<jvar>*)buffer))[i];
        if (i<arrSize-1)
            ret+=separator;
    }
    return ret;
}
void jvar::sort(std::function<bool(jvar&, jvar&)> comparatorFunction)
{
    if (type!=JARRAY)
        return;
    std::sort((*(std::vector<jvar>*)buffer).begin(), (*(std::vector<jvar>*)buffer).end(),comparatorFunction);
}
jvar jvar::filter(std::function<bool(jvar&)> filterFunction)
{
    if (type!=JARRAY)
        (*this);
    jvar ret = ja{};
    std::copy_if ((*(std::vector<jvar>*)buffer).begin()
    , (*(std::vector<jvar>*)buffer).end(), 
    std::back_inserter((*((std::vector<jvar>*)ret.buffer))), filterFunction  );   //[](int i){return i>=0;}
return ret;
}

void jvar::sortAsc()
{
    if (type!=JARRAY)
        return;
    std::sort((*(std::vector<jvar>*)buffer).begin(), (*(std::vector<jvar>*)buffer).end());
}
void jvar::sortDesc()
{
    if (type!=JARRAY)
        return;
    std::sort((*(std::vector<jvar>*)buffer).begin(), (*(std::vector<jvar>*)buffer).end(),std::greater<jvar&>());
}

void jvar::deleteCurrent()
{
    ////logwarn( "My Type: " << type );
    switch(type)
    {
        case JBOOL:
        delete [] ((char*)buffer);
        break;
        case JLONGINTEGER:
        delete [] ((char*)buffer);
        break;
        case JNUMBER:
        delete [] ((char*)buffer);
        break;
        case JSTRING:
        delete ((std::string*)buffer);
        break;
        case JOBJECT:
        delete ((jMapType<std::string,jvar>*)buffer);
        break;
        case JARRAY:
        delete ((std::vector<jvar>*)buffer);
        break;
        case JNULL:  
        break;
    }
    type = JNULL;
}

size_t jvar::eraseItem(std::string key)
{
    switch(type)
    {
        case JOBJECT:
            return ((jMapType<std::string,jvar>*)buffer)->erase(key);
        break;
        default:  
            return 0;
    }
}

void jvar::copyInit(const jvar &source)
{
    type = source.type;
    switch(type)
    {
        case JBOOL:
        buffer = new char[sizeof(bool)];
        memcpy(buffer,source.buffer,sizeof(bool));
        break;
        case JLONGINTEGER:
        buffer = new char[sizeof(long long int)];
        memcpy(buffer,source.buffer,sizeof(long long int));
        break;
        case JNUMBER:
        buffer = new char[sizeof(double)];
        memcpy(buffer,source.buffer,sizeof(double));
        break;
        case JSTRING:
        buffer = (char*) new std::string((*(std::string*)source.buffer));
        break;
        case JOBJECT:
        buffer = (char*) new jMapType<std::string,jvar>((*(jMapType<std::string,jvar>*)source.buffer));
        break;
        case JARRAY:
        buffer = (char*) new std::vector<jvar>((*(std::vector<jvar>*)source.buffer));
        break;
        case JNULL:
        buffer = NULL;
        break;
    }
}
void jvar::replaceAllInternal( std::string &s, const std::string &search, const std::string &replace ) {
    for( size_t pos = 0; ; pos += replace.length() ) {
        // Locate the substring to replace
        pos = s.find( search, pos );
        if( pos == std::string::npos ) break;
        // Replace by erasing and inserting
        s.erase( pos, search.length() );
        s.insert( pos, replace );
    }
}

bool jvar::isNumeric()
{
    if (type==JLONGINTEGER || type==JNUMBER)
        return true;
    return false;
}
bool jvar::isInteger()
{
    if (type==JLONGINTEGER)
        return true;
    return false;
}
bool jvar::isRealNumber()
{
    if (type==JNUMBER)
        return true;
    return false;
}
bool jvar::isString()
{
    if (type==JSTRING)
        return true;
    return false;
}
bool jvar::isArray()
{
    if (type==JARRAY)
        return true;
    return false;
}
bool jvar::isObject()
{
    if (type==JOBJECT)
        return true;
    return false;
}
bool jvar::isNull()
{
    if (type==JNULL)
        return true;
    return false;
}

template <class TType> void jvar::startBuffer( TType val)
{
        buffer = new char[sizeof(val)];
        memcpy(buffer,(const void*)&val,sizeof(val));
}
jvar::jvar(bool val)
{
    startBuffer<bool>(val);
    type = JBOOL;
}
jvar::jvar(char val)
{
    startBuffer<long long int>(val);
    type = JLONGINTEGER;
}
jvar::jvar(int val)
{
    startBuffer<long long int>(val);
    type = JLONGINTEGER;
}
jvar::jvar(long long int val)
{
    startBuffer<long long int>(val);
    type = JLONGINTEGER;
}
jvar::jvar(size_t val)
{
    startBuffer<long long int>(val);
    type = JLONGINTEGER;
}
jvar::jvar(long int val)
{
    startBuffer<long long int>(val);
    type = JLONGINTEGER;
}
jvar::jvar(float val)
{
    startBuffer<double>(val);
    type = JNUMBER;
}
jvar::jvar(double val)
{
    startBuffer<double>(val);
    type = JNUMBER;
}
jvar::jvar(Mnull val)
{
    type=JNULL;
    buffer = NULL;
}
jvar::jvar()
{
    type=JNULL;
    buffer = NULL;
}
jvar::jvar(const std::string &valarg)
{
    buffer = (char*) new std::string(valarg);
    type = JSTRING;
}
jvar::jvar(const std::vector<jvar> &valarg)
{
    buffer = (char*) new std::vector<jvar>(valarg);
    type = JARRAY;
}
jvar::jvar(const jMapType<std::string,jvar> &valarg)
{
    buffer = (char*) new jMapType<std::string,jvar>(valarg);
    type = JOBJECT;
}
jvar::jvar(const char val[])
{
    buffer = (char*) new std::string(val);
    type = JSTRING;
}
jvar::jvar(const jvar &source)//copy constructor
{
    ////logwarn( "Using Copy c" );
    copyInit(source);
}
jvar& jvar::operator=(const jvar& other)//copy assignment
{
    ////logwarn("Using Copy a");
    if(this != &other) {
        deleteCurrent(); 
        copyInit(other);
    }
    return *this;
}

jvar::jvar(jvar &&source) noexcept//move constructor
{
    type=JNULL;
    *this = std::move(source);
}
jvar& jvar::operator=(jvar&& other) noexcept//move assignment
{
    ////logwarn( "Using Move a" );
    if(this != &other) {
        deleteCurrent(); 
        this->type = other.type;
        this->buffer = other.buffer;
        other.type = JNULL;
        other.buffer = NULL;
    }
    return *this;
}



jvar jvar::parsePico(picojson::value obj)
{
    jvar ret;
    if (obj.is<double>())
        return ret = obj.get<double>();
    if (obj.is<int64_t>())
        return ret = obj.get<int64_t>();
    if (obj.is<bool>())
        return ret = obj.get<bool>();
    if (obj.is<std::string>())
        return ret = obj.get<std::string>();
    if (obj.is<picojson::array>())
    {
        std::vector<picojson::value> a = obj.get<picojson::array>();
        size_t max = a.size();
        for (size_t i = 0; i < max; i++)
        {
            ret[i] = parsePico(a[i]);
        }
        if (max==0)
            ret = ja{};
        
        return ret;
    }
    if (obj.is<picojson::object>())
    {
        const picojson::value::object& objit = obj.get<picojson::object>();
        if (objit.begin()==objit.end())
        {
            ret = jo{};
            return ret;
        }
        for (picojson::value::object::const_iterator i = objit.begin();i != objit.end();++i) 
        {
            ret[i->first] = parsePico(i->second);
        }
        return ret;
    }
    return ret;
}
jvar jvar::parseJson(std::string json)
{
    picojson::value root;
    std::string err = picojson::parse(root, json);
    if (!root.is<picojson::object>()) {
        //logwarn( "Jvar Parsing JSON Err, not a JSON OBJECT:",json);
        return MNULL;
    }
    return parsePico(root);
}

std::string jvar::prettyString()
{
    std::string ret;
    std::string find = "\"";
    std::string replace = "\\\"";
    switch(type)
    {
        case JBOOL:
            if ((*(bool*)buffer))
                ret = "true";
            else    
                ret = "false";
            return ret;
        break;
        case JLONGINTEGER:
            return std::to_string((*(long long int*)buffer));
        break;
        case JNUMBER:
            ret = std::to_string((*(double*)buffer));
            ret.erase ( ret.find_last_not_of('0') + 1, std::string::npos );
            if (ret.size()>1 && ret.at(ret.size()-1)=='.')
                ret.erase ( ret.find_last_not_of('.')+1, std::string::npos );
            return ret;
        break;
        case JSTRING:
            ret = (*(std::string*)buffer);
            
            replaceAllInternal(ret,"\\","\\\\");
            replaceAllInternal(ret,"\b","\\b");
            replaceAllInternal(ret,"\f","\\f");
            replaceAllInternal(ret,"\n","\\n");
            replaceAllInternal(ret,"\r","\\r");
            replaceAllInternal(ret,"\"","\\\"");
            replaceAllInternal(ret,"\t","\\t");
            replaceAllInternal(ret,"\t","\\t");
            
            return "\""+ret+"\"";                
        break;
        case JOBJECT:
        {
            jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) buffer;
            jMapType<std::string, jvar>::iterator it = tempm->begin();
            ret = "{\r\n";
            while (it != tempm->end())
            {
                ret += "\t\""+it->first+"\":"+it->second.toJson();
                it++;
                if (it == tempm->end())
                    break;
                ret+=",\r\n";
            }
            ret += "\r\n}\r\n";
            return ret;
        }
        break;
        case JARRAY:
        {
            ret = "[\r\n";
            std::vector<jvar> *temp = ((std::vector<jvar>*) buffer);
            for (size_t i = 0; i < temp->size(); i++)
            {
                ret += "\t"+(*temp)[i].toJson();
                if (i<temp->size()-1)
                    ret+=",\r\n";
            }
            ret += "\r\n]\r\n";
            return ret;
        }
            break;
        case JNULL:
            return "null";    
        break;
    }
    return ret;
}

/*serialize to JSON*/
std::string jvar::toJson()
{
    std::string ret;
    std::string find = "\"";
    std::string replace = "\\\"";
    switch(type)
    {
        case JBOOL:
            if ((*(bool*)buffer))
                ret = "true";
            else    
                ret = "false";
            return ret;
        break;
        case JLONGINTEGER:
            return std::to_string((*(long long int*)buffer));
        break;
        case JNUMBER:
            ret = std::to_string((*(double*)buffer));
            ret.erase ( ret.find_last_not_of('0') + 1, std::string::npos );
            if (ret.size()>1 && ret.at(ret.size()-1)=='.')
                ret.erase ( ret.find_last_not_of('.')+1, std::string::npos );
            return ret;
        break;
        case JSTRING:
            ret = (*(std::string*)buffer);
            
            replaceAllInternal(ret,"\\","\\\\");
            replaceAllInternal(ret,"\b","\\b");
            replaceAllInternal(ret,"\f","\\f");
            replaceAllInternal(ret,"\n","\\n");
            replaceAllInternal(ret,"\r","\\r");
            replaceAllInternal(ret,"\"","\\\"");
            replaceAllInternal(ret,"\t","\\t");
            replaceAllInternal(ret,"\t","\\t");
            
            return "\""+ret+"\"";                
        break;
        case JOBJECT:
        {
            jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) buffer;
            jMapType<std::string, jvar>::iterator it = tempm->begin();
            ret = "{";
            while (it != tempm->end())
            {
                ret += "\""+it->first+"\":"+it->second.toJson();
                it++;
                if (it == tempm->end())
                    break;
                ret+=",";
            }
            ret += "}";
            return ret;
        }
        break;
        case JARRAY:
        {
            ret = "[";
            std::vector<jvar> *temp = ((std::vector<jvar>*) buffer);
            for (size_t i = 0; i < temp->size(); i++)
            {
                ret += (*temp)[i].toJson();
                if (i<temp->size()-1)
                    ret+=",";
            }
            ret += "]";
            return ret;
        }
            break;
        case JNULL:
            return "null";    
        break;
    }
    return ret;
}


/*If you are returning a string, returns the original string, not the serializable one*/
std::string jvar::asString()
{
    switch(type)
    {
        case JSTRING:
            return (*(std::string*)buffer);                
        break;
        default:
            return toJson();
    }
    return "";
}

ji operator<<(const char left[], const jvar &right)
{
    return ji({left,right});
}
std::ostream& operator<<(std::ostream& os, const jvar& right)
{
    os << ((jvar)right).asString();
    return os;
}
jvar & jvar::operator[](const char pos[]) 
{
    if (type!=JOBJECT)  
    {
        deleteCurrent();
        buffer = (char*) new jMapType<std::string,jvar>();
        type = JOBJECT;
    }
    if (type==JOBJECT)
        try{
            return ((jMapType<std::string,jvar>*) buffer)->at(pos);
        }catch(std::out_of_range e)
        {
            ((jMapType<std::string,jvar>*) buffer)->emplace(ji(pos,jvar()));
            return ((jMapType<std::string,jvar>*) buffer)->at(pos);
        }
        //logwarn( "Warning, trying to access non object as object, returning selfval jvar(Something must be wrong!!)" );
        return (*this);
}
jvar & jvar::operator[](std::string pos) 
{
    if (type!=JOBJECT)
    {
        deleteCurrent();
        buffer = (char*) new jMapType<std::string,jvar>();
        type = JOBJECT;
    }
    if (type==JOBJECT)
        try{
            return ((jMapType<std::string,jvar>*) buffer)->at(pos);
        }catch(std::out_of_range e)
        {
            ((jMapType<std::string,jvar>*) buffer)->emplace(ji(pos,jvar()));
            return ((jMapType<std::string,jvar>*) buffer)->at(pos);
        }
        //logwarn( "Warning, trying to access non object as object, returning selfval jvar(Something must be wrong!!)" );
        return (*this);
}

jvar & jvar::allocateArrayWithNElements(size_t n) {
    deleteCurrent();
    buffer = (char*) new std::vector<jvar>(n,jvar());
    type = JARRAY;
    return (*this);
}

void jvar::reserveMemoryForItems(size_t nItems){
    switch(type)
    {
        case JSTRING:
            (*(std::string*)buffer).reserve(nItems);
        break;
        case JARRAY:
            (*((std::vector<jvar>*) buffer)).reserve(nItems);                
        break;
        case JOBJECT:
        {
            #ifdef USE_UNORDERED_MAP
                (*(jMapType<std::string,jvar>*) buffer).reserve(nItems);
            #endif
            return;
        }
        default:
            return;
    }
    return;
}

jvar & jvar::operator[](size_t index) 
{
    if (type!=JARRAY)
    {
        deleteCurrent();
        buffer = (char*) new std::vector<jvar>();
        type = JARRAY;
    }
    if (index<0)
    {
        //logwarn( "WARNING!! You used a negative index, are you crazy!? Returning selfval" );
        return (*this);
    }
    std::vector<jvar> *temp = ((std::vector<jvar>*) buffer);
    if (type==JARRAY)
        try{
        return temp->at(index);
        }catch(std::out_of_range e)
        {
            while (index+1>temp->size())
            {
                temp->emplace_back(jvar());
            }
            return temp->at(index);
        }

        //logwarn( "Warning, trying to access non object as object, returning selfval jvar(Something must be wrong!!)" );
        return (*this);
}

/*only do something if jvar is a string */
jvar jvar::replaceAll( const std::string &search, const std::string &replace ) {
    if (type!=JSTRING)
        return (*this);
    std::string temp = (*(std::string*)buffer);
    replaceAllInternal(temp,search,replace);
    return temp;
}

jvar jvar::replace( const std::string &search, const std::string &replace ) {
    if (type!=JSTRING)
        return (*this);
    size_t pos = (*(std::string*)buffer).find(search);
    if (pos==std::string::npos)
        return (*this);
    std::string currString = (*(std::string*)buffer);
    currString.replace(pos,search.size(),(std::string)replace);
    return currString;
}

jvar jvar::replaceRegex( const std::string &regex, const std::string &replace ) {
    if (type!=JSTRING)
        return (*this);
    std::regex replaceRegex(regex);
    std::string currString = (*(std::string*)buffer);
    currString =std::regex_replace(currString, replaceRegex, replace);

    return currString;
}
jvar jvar::charAt(size_t pos ) {
    if (type!=JSTRING)
        return (*this);
    jvar ret="";
    try{
    ret = (*(std::string*)buffer).at(pos);
    }catch(std::exception e)
    {
        //logwarn(__FILE__,"row",__LINE__, "jvar charAt function",e.what());
    }
    return ret;
}

jvar jvar::toLowerCase()
{
    if (type!=JSTRING)
        return (*this);
    std::string result = (*(std::string*)buffer);
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c){ return std::tolower(c); });
    return result;
}

const char* ws = " \t\n\r\f\v";

// trim from end of string (right)
inline std::string& rtrim(std::string& s, const char* t = ws)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const char* t = ws)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
inline std::string& trimStr(std::string& s, const char* t = ws)
{
    return ltrim(rtrim(s, t), t);
}

jvar jvar::trim()
{
    if (type!=JSTRING)
        return (*this);

    std::string result = (*(std::string*)buffer);
    trimStr(result);

    return result;
}

jvar jvar::toUpperCase()
{
    if (type!=JSTRING)
        return (*this);
    std::string result = (*(std::string*)buffer);
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c){ return std::toupper(c); });
    return result;
}

/*
//utf8, rethinking this
unsigned int toUpper(unsigned int currentCode)
{

}
//utf8, rethinking this
unsigned int toLower(unsigned int currentCode)
{
    //ascii
    if (currentCode>=65 && currentCode <91)
        return currentCode+32;
    //utf-8 accents
    if (currentCode>=192 && currentCode <224)
        return currentCode+32;
    if (currentCode>=256 && currentCode <311 && (currentCode%2)==0 )
        return currentCode+1;
    if (currentCode>=313 && currentCode <328 && (currentCode%2)==1 )
        return currentCode+1;
    if (currentCode>=330 && currentCode <377 && (currentCode%2)==0 )
        return currentCode+1;
    if (currentCode>=377 && currentCode <383 && (currentCode%2)==1 )
        return currentCode+1;
    if (currentCode>=386 && currentCode <383 && (currentCode%2)==1 )
        return currentCode+1;
    if (currentCode>=902 && currentCode <940)
        return currentCode+38;

    return currentCode;
}
*/


/*currently only works with ASCII, plans to expand to UTF-8*/
jvar jvar::toCapitalized()
{
    if (type!=JSTRING)
        return (*this);
    std::string result = (*(std::string*)buffer);
    size_t strLen = result.length();
    for(int i=0;i<strLen;i++)
    {
        unsigned char current = result.at(i);
        //for utf 8 implementation, todo
        if (current>=240 && current<248)
        {
            //unsigned int decVal = 0;
            unsigned char first = result.at(i);
            unsigned char second = result.at(i+1);
            unsigned char third = result.at(i+2);
            unsigned char fourth = result.at(i+3);
            unsigned int decVal = (first%8)*262144 + (second%64)*4096 + (third%64)*64 + (fourth%64);

            result.at(i) = 240 + decVal/262144;
            result.at(i+1) = 128 + (decVal%262144)/4096;
            result.at(i+2) = 128 + ((decVal%262144)%4096)/64;
            result.at(i+3) = 128 + ((decVal%262144)%4096)%64;
            i+=3;
            continue;
        }
        if (current>=224 && current<240)
        {
            //unsigned int decVal = 0;
            unsigned char first = result.at(i);
            unsigned char second = result.at(i+1);
            unsigned char third = result.at(i+2);
            unsigned int decVal = (first%16)*4096 + (second%64)*64 + (third%64);

            
            result.at(i) = 224 + decVal/4096;
            result.at(i+1) = 128 + (decVal%4096)/64;
            result.at(i+2) = 128 + ((decVal%4096)%64);
            i+=2;
            continue;
        }
        if (current>=192 && current<224) 
        {
            unsigned char first = result.at(i);
            unsigned char second = result.at(i+1);
            unsigned int decVal = (first%32)*64 + (second%64);


            result.at(i) = 192 + decVal/64;
            result.at(i+1) = 128 + (decVal%64);
            i+=1;
            continue;
        }

        //ASCII
        if(i==0)
        {
            result.at(0)=toupper(result.at(i));
        }
        if((current>=' ' && current<'A') || current==13 || (current>=9 && current<=10) || (current>' ' && current<'A') 
         ||(current>'Z' && current<'a') ||  (current>'z' && current<'~') )//any separator that is not a letter will make the next character upper case
        {
            if (i+1<strLen)
                result.at(i+1)=toupper(result.at(i+1));
        }
    }
    return result;
}

size_t jvar::size()
{
    switch(type)
    {
        case JSTRING:
            return ((std::string*)buffer)->size();
        case JOBJECT:
            return ((jMapType<std::string,jvar>*)buffer)->size();
        case JARRAY:
            return ((std::vector<jvar>*)buffer)->size();
        default:
            return 0;
    }
}
bool jvar::isEmpty()
{
    return (size()==0);
}

size_t jvar::indexOf(std::string str, size_t start)
{
    switch(type)
    {
        case JSTRING:
            return ((std::string*)buffer)->find(str, start);
        break;
        case JARRAY:
        {
            size_t arrSize = size();
            if (start>=arrSize)
                return std::string::npos;
            for (size_t i = start; i < arrSize; i++)
            {
                if ( (*(ja*)buffer)[i].type==JSTRING && (*(ja*)buffer)[i] == str )
                    return i;
            }
            return std::string::npos;
        }
        break;
        default:
            return std::string::npos;
        break;
    }
}
size_t jvar::lastIndexOf(std::string str, size_t startFromEnd)
{
    size_t useStartFromEnd=startFromEnd;
    if (useStartFromEnd==-1)
        useStartFromEnd=((std::string*)buffer)->size();
    switch(type)
    {
        case JSTRING:
            return ((std::string*)buffer)->find_last_of(str, useStartFromEnd);
        break;
        case JARRAY:
        {
            size_t arrSize = size();
            if (useStartFromEnd==0)
                return std::string::npos;
            for (size_t i = ((arrSize-1)); i >=(arrSize-useStartFromEnd); i--)
            {
                if ( (*(ja*)buffer)[i].type==JSTRING && (*(ja*)buffer)[i] == str )
                    return i;
            }
            return std::string::npos;
        }
        break;
        default:
            return std::string::npos;
        break;
    }
}
jvar jvar::substr(size_t start)
{
    if (type==JSTRING)
        return  ((std::string*)buffer)->substr(start);
    return MNULL;
}
jvar jvar::substr(size_t start, size_t end)
{
    if (type==JSTRING)
        return ((std::string*)buffer)->substr(start,end);
    return MNULL;
}
jvar jvar::entries()
{
    switch(type)
    {
        case JOBJECT:
        {
            size_t counter = 0;
            jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) buffer;
            jMapType<std::string, jvar>::iterator it = tempm->begin();
            jvar ret = ja();
            while (it != tempm->end())
            {
                ret[counter] = ja{ it->first.c_str() , it->second };
                counter++;
                it++;
            }
            return ret;
        }
        break;
        case JARRAY:
        {
            size_t arrSize = size();
            jvar ret = ja();
            for (size_t i = 0; i < arrSize; i++)
            {
                ret[i] = jo{ std::to_string(i).c_str() << (*this) };
            }
            return ret;
        }
        break;
        default:
        {
            jvar ret = ja();
            //ret[(size_t)0] = jo{ "0" << (*this) }; //makes more sense to send empty array
            return ret;
        }
        break;
    }
}
jvar jvar::values()
{
    switch(type)
    {
        case JOBJECT:
        {
            size_t counter = 0;
            jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) buffer;
            jMapType<std::string, jvar>::iterator it = tempm->begin();
            jvar ret = ja();
            while (it != tempm->end())
            {
                ret[counter] = it->second;
                counter++;
                it++;
            }
            return ret;
        }
        break;
        case JARRAY:
        {
            return (*this);
        }
        break;
        default:
        {
            jvar ret = ja();
            ret[(size_t)0] = (*this);
            return ret;
        }
        break;
    }
}
jvar jvar::keys()
{
    switch(type)
    {
        case JOBJECT:
        {
            size_t counter = 0;
            jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) buffer;
            jMapType<std::string, jvar>::iterator it = tempm->begin();
            jvar ret = ja();
            while (it != tempm->end())
            {
                ret[counter] = it->first;
                counter++;
                it++;
            }
            return ret;
        }
        break;
        case JARRAY:
        {
            size_t arrSize = size();
            jvar ret = ja();
            for (size_t i = 0; i < arrSize; i++)
            {
                ret[i] = i;
            }
            return ret;
        }
        break;
        default:
        {
            jvar ret = ja();
            //ret[(size_t)0] = 0;
            return ret;
        }
        break;
    }
}


size_t jvar::memoryFootPrint()
{
    size_t ret = 0;
    switch(type)
    {
        case JBOOL:
            return sizeof(bool)+sizeof(jvar);
        break;
        case JLONGINTEGER:
            return sizeof(long long int)+sizeof(jvar);
        break;
        case JNUMBER:
            return sizeof(double)+sizeof(jvar);
        break;
        case JSTRING:
            return size()+sizeof(jvar)+sizeof(std::string);
        break;
        case JOBJECT:
        {
            jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) buffer;
            jMapType<std::string, jvar>::iterator it = tempm->begin();
            while (it != tempm->end())
            {
                ret += it->second.memoryFootPrint();
                it++;
            }
            return ret + sizeof(jvar)+sizeof(jMapType<std::string, jvar>);
        }
        break;
        case JARRAY:
        {
            std::vector<jvar> *temp = ((std::vector<jvar>*) buffer);
            for (size_t i = 0; i < temp->size(); i++)
            {
                ret += (*temp)[i].memoryFootPrint();
            }
            return ret+ sizeof(jvar)+sizeof(std::vector<jvar>);
        }
        break;
        case JNULL:
            return sizeof(jvar);
        break;
    }
    return sizeof(jvar);
}


long long int jvar::asInteger()
{
    switch(type)
    {
        case JBOOL:
            return ((*(bool*)buffer));
        break;
        case JLONGINTEGER:
            return (*(long long int*)buffer);
        break;
        case JNUMBER:
            return (*(double*)buffer);
        break;
        case JSTRING:
            try{
                return std::stoll((*(std::string*)buffer));
            }
            catch(...)
            {return 0;}     
        break;
        case JOBJECT:
        case JARRAY:
        case JNULL:
            return 0;
        break;
    }
    return 0;
}

double jvar::asDouble()
{
    switch(type)
    {
        case JBOOL:
            return ((*(bool*)buffer));
        break;
        case JLONGINTEGER:
            return (*(long long int*)buffer);
        break;
        case JNUMBER:
            return (*(double*)buffer);
        break;
        case JSTRING:
            try{
                return std::stod((*(std::string*)buffer));
            }
            catch(...)
            {return 0.0;}     
        break;
        case JOBJECT:
        case JARRAY:
        case JNULL:
            return 0.0;
        break;
    }
    return 0.0;
}

bool jvar::asBoolean()
{
    switch(type)
    {
        case JBOOL:
            return ((*(bool*)buffer));
        break;
        case JLONGINTEGER:
            return (*(long long int*)buffer);
        break;
        case JNUMBER:
            return (*(double*)buffer);
        break;
        case JSTRING:
        case JOBJECT:
        case JARRAY:
        if (size()>0)
                return true;
            return false; 
        break;
        case JNULL:
            return false;
        break;
    }
    return false;
}



jvar operator+(jvar left, const jvar &right)
{
    switch(left.type)
    {
        case JNUMBER:
            switch(right.type)
            {
                case JLONGINTEGER:
                    (*(double*) left.buffer) = (*(double*) left.buffer) + ((jvar)right).asInteger() ;
                break;
                case JNUMBER:
                    (*(double*) left.buffer) = (*(double*) left.buffer) + ((jvar)right).asDouble() ;
                break;
                case JSTRING:
                {
                    double temp;
                    temp = (*((double*)left.buffer));
                    left.deleteCurrent();
                    left.type = JSTRING;
                    left.buffer = (char*) new std::string(std::to_string(temp)+(*(std::string*)right.buffer));
                }
                break;
                case JBOOL:
                case JOBJECT:
                case JARRAY:
                    left = right + left;
                break;
                case JNULL:
                    //logwarn( "Warning, tried to add JNull with JNumber(doing nothing)" );
                break;
            }
        break;
        case JLONGINTEGER:
            switch(right.type)
            {
                case JLONGINTEGER:
                    (*(long long int*) left.buffer) = (*(long long int*) left.buffer) + ((jvar)right).asInteger() ;
                break;
                case JNUMBER:
                    left = (double)((double)left.asInteger() + ((jvar)right).asDouble()) ;
                break;
                case JSTRING:
                {
                    long long int tempint;
                    tempint = (*((long long int*)left.buffer));
                    left.deleteCurrent();
                    left.type = JSTRING;
                    left.buffer = (char*) new std::string(std::to_string(tempint)+(*(std::string*)right.buffer));
                }
                break;
                case JBOOL:
                case JOBJECT:
                case JARRAY:
                    left = right + left;
                break;
                case JNULL:
                    //logwarn( "Warning, tried to add JNull with JLONGINTEGER(doing nothing)" );
                break;
            }
        break;
        case JBOOL:
            switch(right.type)
            {
                case JLONGINTEGER:
                    (*(bool*) left.buffer) = ( (*(bool*) left.buffer) || ((jvar)right).asBoolean() );
                    //logwarn( "Warning, tried to add JLONGINTEGER with JBOOL(converting long long int to boolean)" );
                break;
                case JNUMBER:
                    (*(bool*) left.buffer) = ( (*(bool*) left.buffer) || ((jvar)right).asBoolean() );
                    //logwarn( "Warning, tried to add JNUMBER with JBOOL(converting double to boolean)" );
                break;
                case JSTRING:
                {
                    std::string tempstr;
                    if ((*(bool*) left.buffer))
                        tempstr = "true";
                    else
                        tempstr = "false";
                    left.deleteCurrent();
                    left.type = JSTRING;
                    left.buffer = (char*) new std::string(tempstr+(*(std::string*)right.buffer));
                }
                break;
                case JBOOL:
                    (*(bool*) left.buffer) = ( (*(bool*) left.buffer) || (*(bool*) right.buffer) );
                break;
                case JOBJECT:
                case JARRAY:
                    left = right + left;
                break;
                case JNULL:
                    //logwarn( "Warning, tried to add JNull with JBOOL(doing nothing)" );
                break;
            }
        break;
        case JSTRING:
            (*(std::string*) left.buffer) = (*(std::string*) left.buffer) + ((jvar)right).asString();
        break;
        case JNULL:
            switch(right.type)
            {
                case JSTRING:
                    {
                        std::string tempstr;
                        tempstr = "null";
                        left.type = JSTRING;
                        left.buffer = (char*) new std::string(tempstr+(*(std::string*)right.buffer));
                    }
                break;
                default: //jnull is the same as empty, so adding it with something else will make it the same as the other thing 
                    left = right;
                break;
            }
        break;
        case JARRAY:
            switch(right.type)
            {
                case JLONGINTEGER:
                case JNUMBER:
                {
                    size_t arrsize = left.size();
                    for (size_t i = 0; i < arrsize; i++)
                    {
                        left[i] = left[i] + right;
                    }
                }
                break;
                case JARRAY:
                {
                    size_t otherSize = ((jvar)right).size();
                    size_t useSize = left.size();
                    if (otherSize<useSize)
                        useSize = otherSize;
                    for (size_t i = 0; i < useSize; i++)
                    {
                        left[i] = left[i] + ((jvar)right)[i];
                    }
                }
                break;
                case JBOOL:
                {
                    size_t arrsize = left.size();
                    for (size_t i = 0; i < arrsize; i++)
                    {
                        left[i] = left[i].asBoolean() || (*((bool*)right.buffer));
                    }
                }
                case JSTRING:
                    left = left.asString() + right;
                break;
                default:
                    //logwarn( "Warning, tried to add JArray with a value(any of jbool,jnull,jstr,jobject doing nothing)" );
                break;
            }
        break;
        case JOBJECT:
            switch(right.type)
            {
                case JLONGINTEGER:
                case JNUMBER:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    while (it != tempm->end())
                    {
                        it->second = it->second + right;
                        it++;
                    }
                }
                break;
                case JOBJECT:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) right.buffer;
                    jMapType<std::string, jvar> *templeft =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    jMapType<std::string, jvar>::iterator leftit = templeft->begin();
                    while (it != tempm->end())
                    {
                        leftit = templeft->find(it->first);
                        if (leftit!=templeft->end())
                            leftit->second = leftit->second + it->second;
                        it++;
                    }
                }
                break;
                case JBOOL:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    while (it != tempm->end())
                    {
                        it->second = it->second.asBoolean() || (*((bool*)right.buffer));
                        it++;
                    }
                }
                case JSTRING:
                    left = left.asString() + right;
                break;
                default:
                    //logwarn( "Warning, tried to add JObject with a value(any of jbool,jnull,jstr,jarray doing nothing)" );
                break;
            }
        break;
    }
    return left;
}

//merge
jvar &operator<<(jvar &left, const jvar &right)
{
    switch (left.type)
    {
        case JARRAY:
        {
            switch (right.type)
            {
                case JARRAY:
                {
                    (*((ja*)left.buffer)).insert( (*((ja*)left.buffer)).end(), (*((ja*)right.buffer)).begin(), (*((ja*)right.buffer)).end() );
                    return left;
                }
                case JOBJECT:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) right.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    size_t start = left.size();
                    while (it != tempm->end())
                    {
                        left[start] = jo {
                            it->first.c_str() << jv it->second
                        };
                        it++;
                        start++;
                    }
                    return left;
                }
                default:
                {
                    left[left.size()] = right;
                    return left;
                }
            }
            break;
        }
        case JOBJECT:
        {
            switch (right.type)
            {
                case JARRAY:
                {
                    size_t asize = ((jvar)right).size();
                    for (size_t i = 0; i < asize; i++)
                    {
                        left[std::to_string(i)] = ((jvar)right)[i];
                    }
                    return left;
                }
                case JOBJECT:
                {
                    jMapType<std::string, jvar> *tempright =(jMapType<std::string, jvar> *) right.buffer;
                    jMapType<std::string, jvar>::iterator it = tempright->begin();
                    while (it != tempright->end())
                    {
                        left[it->first.c_str()] = it->second;
                        it++;
                    }
                    return left;
                }
                default:
                {
                    left[(jv right).asString()] = right;
                    return left;
                }
            }
            break;
        }
        default:
        {
            jvar ret = ja();
            ret << left;
            ret << right;
            left = ret;
            return left;
        }
    } 
}


jvar& jvar::operator+=(const jvar& other)
{
    (*this) = (*this) + other;
    return (*this);
}
jvar& jvar::operator-=(const jvar& other)
{
    (*this) = (*this) - other;
    return (*this);
}
jvar& jvar::operator*=(const jvar& other)
{
    (*this) = (*this) * other;
    return (*this);
}
jvar& jvar::operator/=(const jvar& other)
{
    (*this) = (*this) / other;
    return (*this);
}

jvar operator-(jvar left, const jvar &right)
{
    switch(left.type)
    {
        case JNUMBER:
            switch(right.type)
            {
                case JLONGINTEGER:
                    (*(double*) left.buffer) = (*(double*) left.buffer) - ((jvar)right).asInteger() ;
                break;
                case JNUMBER:
                    (*(double*) left.buffer) = (*(double*) left.buffer) - ((jvar)right).asDouble() ;
                break;
                default:
                    //logwarn( "Warning, tried to subtract JNUMBER with type" , jvarTypesNames[right.type] );
                break;
            }
        break;
        case JLONGINTEGER:
            switch(right.type)
            {
                case JLONGINTEGER:
                    (*(long long int*) left.buffer) = (*(long long int*) left.buffer) - ((jvar)right).asInteger() ;
                break;
                case JNUMBER:
                    left = (double)((double)left.asInteger() - ((jvar)right).asDouble()) ;
                break;
                default:
                    //logwarn( "Warning, tried to subtract JLONGINT with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JBOOL:
            //logwarn( "Warning, tried to subtract JBOOL with type",jvarTypesNames[right.type] );
        break;
        case JSTRING:
            //logwarn( "Warning, tried to subtract JSTRING with type",jvarTypesNames[right.type] );
        break;
        case JNULL:
            //logwarn( "Warning, tried to subtract JNULL with type",jvarTypesNames[right.type] );
        break;
        case JARRAY:
            switch(right.type)
            {
                case JLONGINTEGER:
                case JNUMBER:
                {
                    size_t arrsize = left.size();
                    for (size_t i = 0; i < arrsize; i++)
                    {
                        left[i] = left[i] - right;
                    }
                }
                break;
                case JARRAY:
                {
                    size_t otherSize = ((jvar)right).size();
                    size_t useSize = left.size();
                    if (otherSize<useSize)
                        useSize = otherSize;
                    for (size_t i = 0; i < useSize; i++)
                    {
                        left[i] = left[i] - ((jvar)right)[i];
                    }
                }
                break;
                default:
                    //logwarn( "Warning, tried to subtract JARRAY with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JOBJECT:
            switch(right.type)
            {
                case JLONGINTEGER:
                case JNUMBER:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    while (it != tempm->end())
                    {
                        it->second = it->second - right;
                        it++;
                    }
                }
                break;
                case JOBJECT:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) right.buffer;
                    jMapType<std::string, jvar> *templeft =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    jMapType<std::string, jvar>::iterator leftit = templeft->begin();
                    while (it != tempm->end())
                    {
                        leftit = templeft->find(it->first);
                        if (leftit!=templeft->end())
                            leftit->second = leftit->second - it->second;
                        it++;
                    }
                }
                break;
                default:
                    //logwarn( "Warning, tried to subtract JOBJECT with type",jvarTypesNames[right.type] );
                break;
            }
        break;
    }
    return left;
}   




jvar operator*(jvar left, const jvar &right)
{
    switch(left.type)
    {
        case JNUMBER:
            switch(right.type)
            {
                case JLONGINTEGER:
                    (*(double*) left.buffer) = (*(double*) left.buffer) * ((jvar)right).asInteger() ;
                break;
                case JNUMBER:
                    (*(double*) left.buffer) = (*(double*) left.buffer) * ((jvar)right).asDouble() ;
                break;
                case JBOOL:
                case JOBJECT:
                case JARRAY:
                    left = right * left;
                break;
                default:
                    //logwarn( "Warning, tried to multiply JNUMBER with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JLONGINTEGER:
            switch(right.type)
            {
                case JLONGINTEGER:
                    (*(long long int*) left.buffer) = (*(long long int*) left.buffer) * ((jvar)right).asInteger() ;
                break;
                case JNUMBER:
                    left = (double)((double)left.asInteger() * ((jvar)right).asDouble()) ;
                break;
                case JBOOL:
                case JOBJECT:
                case JARRAY:
                    left = right * left;
                break;
                default:
                    //logwarn( "Warning, tried to multiply JLONGINTEGER with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JBOOL:
            switch(right.type)
            {
                case JLONGINTEGER:
                    (*(bool*) left.buffer) = ( (*(bool*) left.buffer) && ((jvar)right).asBoolean() );
                    //logwarn( "Warning, tried to and JLONGINTEGER with JBOOL(converting long long int to boolean)" );
                break;
                case JNUMBER:
                    (*(bool*) left.buffer) = ( (*(bool*) left.buffer) && ((jvar)right).asBoolean() );
                    //logwarn( "Warning, tried to and JNUMBER with JBOOL(converting double to boolean)" );
                break;
                case JBOOL:
                    (*(bool*) left.buffer) = ( (*(bool*) left.buffer) && (*(bool*) right.buffer) );
                break;
                case JOBJECT:
                case JARRAY:
                    left = right * left;
                break;
                default:
                    //logwarn( "Warning, tried to and JBOOL with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JSTRING:
            //logwarn( "Warning, tried to multiply JSTRING with type",jvarTypesNames[right.type] );
        break;
        case JNULL:
            //logwarn( "Warning, tried to multiply JNULL with type",jvarTypesNames[right.type] );
        break;
        case JARRAY:
            switch(right.type)
            {
                case JLONGINTEGER:
                case JNUMBER:
                {
                    size_t arrsize = left.size();
                    for (size_t i = 0; i < arrsize; i++)
                    {
                        left[i] = left[i] * right;
                    }
                }
                break;
                case JARRAY:
                {
                    size_t otherSize = ((jvar)right).size();
                    size_t useSize = left.size();
                    if (otherSize<useSize)
                        useSize = otherSize;
                    for (size_t i = 0; i < useSize; i++)
                    {
                        left[i] = left[i] * ((jvar)right)[i];
                    }
                }
                break;
                case JBOOL:
                {
                    size_t arrsize = left.size();
                    for (size_t i = 0; i < arrsize; i++)
                    {
                        left[i] = left[i].asBoolean() && right;
                    }
                }
                default:
                    //logwarn( "Warning, tried to multiply JARRAY with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JOBJECT:
            switch(right.type)
            {
                case JLONGINTEGER:
                case JNUMBER:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    while (it != tempm->end())
                    {
                        it->second = it->second * right;
                        it++;
                    }
                }
                break;
                case JOBJECT:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) right.buffer;
                    jMapType<std::string, jvar> *templeft =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    jMapType<std::string, jvar>::iterator leftit = templeft->begin();
                    while (it != tempm->end())
                    {
                        leftit = templeft->find(it->first);
                        if (leftit!=templeft->end())
                            leftit->second = leftit->second * it->second;
                        it++;
                    }
                }
                break;
                case JBOOL:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    while (it != tempm->end())
                    {
                        it->second = it->second.asBoolean() && right;
                        it++;
                    }
                }
                default:
                    //logwarn( "Warning, tried to add JObject with a value(any of jbool,jnull,jstr,jarray doing nothing)" );
                break;
            }
        break;
    }
    return left;
}


jvar operator/(jvar left, const jvar &right)
{
    switch(left.type)
    {
        case JNUMBER:
            switch(right.type)
            {
                case JLONGINTEGER:
                    (*(double*) left.buffer) = (*(double*) left.buffer) / ((jvar)right).asInteger() ;
                break;
                case JNUMBER:
                    (*(double*) left.buffer) = (*(double*) left.buffer) / ((jvar)right).asDouble() ;
                break;
                default:
                    //logwarn( "Warning, tried to divide JNUMBER with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JLONGINTEGER:
            switch(right.type)
            {
                case JLONGINTEGER:
                    (*(long long int*) left.buffer) = (*(long long int*) left.buffer) / ((jvar)right).asInteger() ;
                break;
                case JNUMBER:
                    left = (double)((double)left.asInteger() / ((jvar)right).asDouble()) ;
                break;
                default:
                    //logwarn( "Warning, tried to divide JLONGINT with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JBOOL:
            //logwarn( "Warning, tried to divide JBOOL with type",jvarTypesNames[right.type] );
        break;
        case JSTRING:
            //logwarn( "Warning, tried to divide JSTRING with type",jvarTypesNames[right.type] );
        break;
        case JNULL:
            //logwarn( "Warning, tried to divide JNULL with type",jvarTypesNames[right.type] );
        break;
        case JARRAY:
            switch(right.type)
            {
                case JLONGINTEGER:
                case JNUMBER:
                {
                    size_t arrsize = left.size();
                    for (size_t i = 0; i < arrsize; i++)
                    {
                        left[i] = left[i] / right;
                    }
                }
                break;
                case JARRAY:
                {
                    size_t otherSize = ((jvar)right).size();
                    size_t useSize = left.size();
                    if (otherSize<useSize)
                        useSize = otherSize;
                    for (size_t i = 0; i < useSize; i++)
                    {
                        left[i] = left[i] / ((jvar)right)[i];
                    }
                }
                break;
                default:
                    //logwarn( "Warning, tried to divide JARRAY with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JOBJECT:
            switch(right.type)
            {
                case JLONGINTEGER:
                case JNUMBER:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    while (it != tempm->end())
                    {
                        it->second = it->second / right;
                        it++;
                    }
                }
                break;
                case JOBJECT:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) right.buffer;
                    jMapType<std::string, jvar> *templeft =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    jMapType<std::string, jvar>::iterator leftit = templeft->begin();
                    while (it != tempm->end())
                    {
                        leftit = templeft->find(it->first);
                        if (leftit!=templeft->end())
                            leftit->second = leftit->second / it->second;
                        it++;
                    }
                }
                break;
                default:
                    //logwarn( "Warning, tried to divide JOBJECT with type",jvarTypesNames[right.type] );
                break;
            }
        break;
    }
    return left;
}   




jvar operator%(jvar left, const jvar &right)
{
    switch(left.type)
    {
        case JNUMBER:
            //logwarn( "Warning, getting remainder of division of real JNUMBER with type",jvarTypesNames[right.type] , " converting to integer" );
            switch(right.type)
            {
                case JNUMBER:
                    //logwarn( "Warning, getting remainder of division of real JNUMBER with type",jvarTypesNames[right.type] , " converting to integer" );
                case JLONGINTEGER:
                    (*(double*) left.buffer) = left.asInteger() % ((jvar)right).asInteger() ;
                break;
                default:
                    //logwarn( "Warning, tried to divide JNUMBER with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JLONGINTEGER:
            switch(right.type)
            {
                case JNUMBER:
                    //logwarn( "Warning, getting remainder of division of Integer by real JNUMBER converting to integer" );
                case JLONGINTEGER:
                    (*(long long int*) left.buffer) = left.asInteger() % ((jvar)right).asInteger() ;
                break;
                default:
                    //logwarn( "Warning, tried to divide JLONGINT with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JBOOL:
            //logwarn( "Warning, tried getting remainder of division JBOOL with type",jvarTypesNames[right.type] );
        break;
        case JSTRING:
            //logwarn( "Warning, tried getting remainder of division JSTRING with type",jvarTypesNames[right.type] );
        break;
        case JNULL:
            //logwarn( "Warning, tried getting remainder of division JNULL with type",jvarTypesNames[right.type] );
        break;
        case JARRAY:
            switch(right.type)
            {
                case JLONGINTEGER:
                case JNUMBER:
                {
                    size_t arrsize = left.size();
                    for (size_t i = 0; i < arrsize; i++)
                    {
                        left[i] = left[i] % right;
                    }
                }
                break;
                case JARRAY:
                {
                    size_t otherSize = ((jvar)right).size();
                    size_t useSize = left.size();
                    if (otherSize<useSize)
                        useSize = otherSize;
                    for (size_t i = 0; i < useSize; i++)
                    {
                        left[i] = left[i] % ((jvar)right)[i];
                    }
                }
                break;
                default:
                    //logwarn( "Warning, tried getting remainder of division JARRAY with type",jvarTypesNames[right.type] );
                break;
            }
        break;
        case JOBJECT:
            switch(right.type)
            {
                case JLONGINTEGER:
                case JNUMBER:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    while (it != tempm->end())
                    {
                        it->second = it->second % right;
                        it++;
                    }
                }
                break;
                case JOBJECT:
                {
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) right.buffer;
                    jMapType<std::string, jvar> *templeft =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    jMapType<std::string, jvar>::iterator leftit = templeft->begin();
                    while (it != tempm->end())
                    {
                        leftit = templeft->find(it->first);
                        if (leftit!=templeft->end())
                            leftit->second = leftit->second % it->second;
                        it++;
                    }
                }
                break;
                default:
                    //logwarn( "Warning, tried getting remainder of division JOBJECT with type",jvarTypesNames[right.type] );
                break;
            }
        break;
    }
    return left;
}   





bool operator==(const jvar &left, const jvar &right)
{
    switch(left.type)
    {
        case JNUMBER:
            switch(right.type)
            {
                case JNUMBER:
                    return (*(double*)left.buffer) == (*(double*)right.buffer);
                break;
                case JLONGINTEGER:
                    return (*(double*)left.buffer) == (*(long long int*)right.buffer);
                break;
                case JBOOL:
                    return ((jvar)left).asBoolean() == (*(bool*)right.buffer);
                break;
                default:
                    return false;
            }
        break;
        case JLONGINTEGER:
            switch(right.type)
            {
                case JNUMBER:
                    return (*(long long int*)left.buffer) == (*(double*)right.buffer);
                break;
                case JLONGINTEGER:
                    return (*(long long int*)left.buffer) == (*(long long int*)right.buffer);
                break;
                case JBOOL:
                    return ((jvar)left).asBoolean() == (*(bool*)right.buffer);
                break;
                default:
                    return false;
            }
        break;
        case JBOOL:
            switch(right.type)
            {
                case JNULL:
                    return false;
                break;
                default:
                    return (*(long long int*)left.buffer) == ((jvar)right).asBoolean();
            }
        break;
        case JSTRING:
            switch(right.type)
            {
                case JSTRING:
                    return (*(std::string*)left.buffer) == (*(std::string*)right.buffer);
                break;
                case JBOOL:
                    return ((jvar)left).asBoolean() == (*(bool*)right.buffer);
                default:
                    return false;
            }
        break;
        case JNULL:
            switch(right.type)
            {
                case JNULL:
                    return true;
                break;
                default:
                    return false;
            }
        break;
        case JARRAY:
            switch(right.type)
            {
                case JARRAY:
                {
                    size_t arrSize = ((jvar)left).size();
                    if ( arrSize!=((jvar)right).size() )
                        return false;
                    
                    for (size_t i = 0; i < arrSize; i++)
                    {
                        if ( ! ( (*(std::vector<jvar>*)left.buffer)[i] == (*(std::vector<jvar>*)right.buffer)[i]) )
                            return false;
                    }
                    return true;
                }
                break;
                case JBOOL:
                    return ((jvar)left).asBoolean() == (*(bool*)right.buffer);
                default:
                    return false;
            }
        break;
        case JOBJECT:
            switch(right.type)
            {
                case JOBJECT:
                {
                    size_t mapSize = ((jvar)left).size();
                    if ( mapSize!=((jvar)right).size() )
                        return false;
                    
                    jMapType<std::string, jvar> *tempm =(jMapType<std::string, jvar> *) right.buffer;
                    jMapType<std::string, jvar> *templeft =(jMapType<std::string, jvar> *) left.buffer;
                    jMapType<std::string, jvar>::iterator it = tempm->begin();
                    jMapType<std::string, jvar>::iterator leftit = templeft->begin();
                    while (it != tempm->end())
                    {
                        leftit = templeft->find(it->first);
                        if (leftit==templeft->end())
                            return false;
                        
                        if ( !(leftit->second == it->second) )
                            return false;
                        it++;
                    }
                    return true;
                }
                break;
                case JBOOL:
                    return ((jvar)left).asBoolean() == (*(bool*)right.buffer);
                default:
                    return false;
            }
        break;
    }
    return false;
}
bool operator!=(const jvar &left, const jvar &right)
{
    return !(left==right);
}


bool operator>(const jvar &left, const jvar &right)
{
    switch(left.type)
    {
        case JNUMBER:
            switch(right.type)
            {
                case JNUMBER:
                    return (*(double*)left.buffer) > (*(double*)right.buffer);
                break;
                case JLONGINTEGER:
                    return (*(double*)left.buffer) > (*(long long int*)right.buffer);
                break;
                case JBOOL:
                    return ((jvar)left).asBoolean() > (*(bool*)right.buffer);
                break;
                case JNULL:
                    return ((jvar)left).asBoolean() > false;
                break;
                default:
                    return false;
            }
        break;
        case JLONGINTEGER:
            switch(right.type)
            {
                case JNUMBER:
                    return (*(long long int*)left.buffer) > (*(double*)right.buffer);
                break;
                case JLONGINTEGER:
                    return (*(long long int*)left.buffer) > (*(long long int*)right.buffer);
                break;
                case JBOOL:
                    return ((jvar)left).asBoolean() > (*(bool*)right.buffer);
                break;
                case JNULL:
                    return ((jvar)left).asBoolean() > false;
                break;
                default:
                    return false;
            }
        break;
        case JBOOL:
            switch(right.type)
            {
                case JNULL:
                    return ((jvar)left).asBoolean() > false;
                break;
                default:
                    return (*(long long int*)left.buffer) > ((jvar)right).asBoolean();
            }
            
        break;
        case JSTRING:
            switch(right.type)
            {
                case JSTRING:
                    return (*(std::string*)left.buffer) > (*(std::string*)right.buffer);
                break;
                case JBOOL:
                    return ((jvar)left).asBoolean() > (*(bool*)right.buffer);
                case JNULL:
                    return ((jvar)left).asBoolean() > false;
                break;
                default:
                    return ((jvar)left).size() > ((jvar)right).size();
            }
        break;
        case JNULL:
            switch(right.type)
            {
                case JNULL:
                    return false;
                break;
                default:
                    return false > ((jvar)right).asBoolean();
                break;
            }
        break;
        case JARRAY:
        case JOBJECT:
            return ((jvar)left).size() > ((jvar)right).size();
        break;
    }
    return false;
}
bool operator<(const jvar &left, const jvar &right)
{
    if (left>right)
        return false;
    if (left==right)
        return false;
    return true;
}
bool operator<=(const jvar &left, const jvar &right)
{
    if (left>right)
        return false;
    return true;
}
bool operator>=(const jvar &left, const jvar &right)
{
    if (left<right)
        return false;
    return true;
}
/*
char operator<=>(const jvar &left, const jvar &right)
{
    if (left==right)
        return 0;
    if (left>right)
        return 1;
    return -1;
}
*/



jvar operator+(jvar left, const double &right)
{
    left = left + jvar(right);
    return left;
}
jvar operator+(jvar left, const int &right)
{
    left = left + jvar(right);
    return left;
}
jvar operator+(jvar left, const long int &right)
{
    left = left + jvar((int)right);
    return left;
}
jvar operator+(jvar left, const long long int &right)
{
    left = left + jvar(right);
    return left;
}
jvar operator+(jvar left, const short int &right)
{
    left = left + jvar(right);
    return left;
}
jvar operator+(jvar left, const float &right)
{
    left = left + jvar(right);
    return left;
}
jvar operator+(jvar left, const char &right)
{
    left = left + jvar(right);
    return left;
}
jvar operator+(jvar left, const char right[])
{
    left = left + jvar(right);
    return left;
}
jvar operator+(jvar left, const std::string right)
{
    left = left + jvar(right);
    return left;
}
jvar operator+(double left, const jvar &right)
{
    jvar ret = jvar(left) + right;
    return ret;
}
jvar operator+(float left, const jvar &right)
{
    jvar ret = jvar(left) + right;
    return ret;
}
jvar operator+(char left, const jvar &right)
{
    jvar ret = jvar(left) + right;
    return ret;
}
jvar operator+(short int left, const jvar &right)
{
    jvar ret = jvar(left) + right;
    return ret;
}
jvar operator+(int left, const jvar &right)
{
    jvar ret = jvar(left) + right;
    return ret;
}
jvar operator+(long int left, const jvar &right)
{
    jvar ret = jvar((int)left) + right;
    return ret;
}
jvar operator+(long long int left, const jvar &right)
{
    jvar ret = jvar(left) + right;
    return ret;
}
jvar operator+(const char left[], const jvar &right)
{
    jvar ret = jvar(left) + right;
    return ret;
}
jvar operator+(const std::string left, const jvar &right)
{
    jvar ret = jvar(left) + right;
    return ret;
}


jvar operator-(jvar left, const double &right)
{
    left = left - jvar(right);
    return left;
}
jvar operator-(jvar left, const int &right)
{
    left = left - jvar(right);
    return left;
}
jvar operator-(jvar left, const long int &right)
{
    left = left - jvar((int)right);
    return left;
}
jvar operator-(jvar left, const long long int &right)
{
    left = left - jvar(right);
    return left;
}
jvar operator-(jvar left, const short int &right)
{
    left = left - jvar(right);
    return left;
}
jvar operator-(jvar left, const float &right)
{
    left = left - jvar(right);
    return left;
}
jvar operator-(jvar left, const char &right)
{
    left = left - jvar(right);
    return left;
}
jvar operator-(double left, const jvar &right)
{
    jvar ret = jvar(left) - right;
    return ret;
}
jvar operator-(float left, const jvar &right)
{
    jvar ret = jvar(left) - right;
    return ret;
}
jvar operator-(char left, const jvar &right)
{
    jvar ret = jvar(left) - right;
    return ret;
}
jvar operator-(short int left, const jvar &right)
{
    jvar ret = jvar(left) - right;
    return ret;
}
jvar operator-(int left, const jvar &right)
{
    jvar ret = jvar(left) - right;
    return ret;
}
jvar operator-(long int left, const jvar &right)
{
    jvar ret = jvar((int)left) - right;
    return ret;
}
jvar operator-(long long int left, const jvar &right)
{
    jvar ret = jvar(left) - right;
    return ret;
}


jvar operator*(jvar left, const double &right)
{
    left = left * jvar(right);
    return left;
}
jvar operator*(jvar left, const int &right)
{
    left = left * jvar(right);
    return left;
}
jvar operator*(jvar left, const long int &right)
{
    left = left * jvar((int)right);
    return left;
}
jvar operator*(jvar left, const long long int &right)
{
    left = left * jvar(right);
    return left;
}
jvar operator*(jvar left, const short int &right)
{
    left = left * jvar(right);
    return left;
}
jvar operator*(jvar left, const float &right)
{
    left = left * jvar(right);
    return left;
}
jvar operator*(jvar left, const char &right)
{
    left = left * jvar(right);
    return left;
}
jvar operator*(double left, const jvar &right)
{
    jvar ret = jvar(left) * right;
    return ret;
}
jvar operator*(float left, const jvar &right)
{
    jvar ret = jvar(left) * right;
    return ret;
}
jvar operator*(char left, const jvar &right)
{
    jvar ret = jvar(left) * right;
    return ret;
}
jvar operator*(short int left, const jvar &right)
{
    jvar ret = jvar(left) * right;
    return ret;
}
jvar operator*(int left, const jvar &right)
{
    jvar ret = jvar(left) * right;
    return ret;
}
jvar operator*(long int left, const jvar &right)
{
    jvar ret = jvar((int)left) * right;
    return ret;
}
jvar operator*(long long int left, const jvar &right)
{
    jvar ret = jvar(left) * right;
    return ret;
}


jvar operator/(jvar left, const double &right)
{
    left = left / jvar(right);
    return left;
}
jvar operator/(jvar left, const int &right)
{
    left = left / jvar(right);
    return left;
}
jvar operator/(jvar left, const long int &right)
{
    left = left / jvar((int)right);
    return left;
}
jvar operator/(jvar left, const long long int &right)
{
    left = left / jvar(right);
    return left;
}
jvar operator/(jvar left, const short int &right)
{
    left = left / jvar(right);
    return left;
}
jvar operator/(jvar left, const float &right)
{
    left = left / jvar(right);
    return left;
}
jvar operator/(jvar left, const char &right)
{
    left = left / jvar(right);
    return left;
}
jvar operator/(double left, const jvar &right)
{
    jvar ret = jvar(left) / right;
    return ret;
}
jvar operator/(float left, const jvar &right)
{
    jvar ret = jvar(left) / right;
    return ret;
}
jvar operator/(char left, const jvar &right)
{
    jvar ret = jvar(left) / right;
    return ret;
}
jvar operator/(short int left, const jvar &right)
{
    jvar ret = jvar(left) / right;
    return ret;
}
jvar operator/(int left, const jvar &right)
{
    jvar ret = jvar(left) / right;
    return ret;
}
jvar operator/(long int left, const jvar &right)
{
    jvar ret = jvar((int)left) / right;
    return ret;
}
jvar operator/(long long int left, const jvar &right)
{
    jvar ret = jvar(left) / right;
    return ret;
}

jvar operator%(jvar left, const double &right)
{
    left = left % jvar(right);
    return left;
}
jvar operator%(jvar left, const int &right)
{
    left = left % jvar(right);
    return left;
}
jvar operator%(jvar left, const long int &right)
{
    left = left % jvar((int)right);
    return left;
}
jvar operator%(jvar left, const long long int &right)
{
    left = left % jvar(right);
    return left;
}
jvar operator%(jvar left, const short int &right)
{
    left = left % jvar(right);
    return left;
}
jvar operator%(jvar left, const float &right)
{
    left = left % jvar(right);
    return left;
}
jvar operator%(jvar left, const char &right)
{
    left = left % jvar(right);
    return left;
}
jvar operator%(double left, const jvar &right)
{
    jvar ret = jvar(left) % right;
    return ret;
}
jvar operator%(float left, const jvar &right)
{
    jvar ret = jvar(left) % right;
    return ret;
}
jvar operator%(char left, const jvar &right)
{
    jvar ret = jvar(left) % right;
    return ret;
}
jvar operator%(short int left, const jvar &right)
{
    jvar ret = jvar(left) % right;
    return ret;
}
jvar operator%(int left, const jvar &right)
{
    jvar ret = jvar(left) % right;
    return ret;
}
jvar operator%(long int left, const jvar &right)
{
    jvar ret = jvar((int)left) % right;
    return ret;
}
jvar operator%(long long int left, const jvar &right)
{
    jvar ret = jvar(left) % right;
    return ret;
}
bool operator==(jvar left, const bool &right)
{
    return left == jvar(right);
}
bool operator==(jvar left, const double &right)
{
    return left == jvar(right);
}
bool operator==(jvar left, const int &right)
{
    return left == jvar(right);
}
bool operator==(jvar left, const long int &right)
{
    return left == jvar(right);
}
bool operator==(jvar left, const long long int &right)
{
    return left == jvar(right);
}
bool operator==(jvar left, const short int &right)
{
    return left == jvar(right);
}
bool operator==(jvar left, const float &right)
{
    return left == jvar(right);
}
bool operator==(jvar left, const char &right)
{
    return left == jvar(right);
}
bool operator==(jvar left, const char right[])
{
    return left == jvar(right);
}
bool operator==(bool left, const jvar &right)
{
    return jvar(left) == right;
}
bool operator==(double left, const jvar &right)
{
    return jvar(left) == right;
}
bool operator==(float left, const jvar &right)
{
    return jvar(left) == right;
}
bool operator==(char left, const jvar &right)
{
    return jvar(left) == right;
}
bool operator==(short int left, const jvar &right)
{
    return jvar(left) == right;
}
bool operator==(int left, const jvar &right)
{
    return jvar(left) == right;
}
bool operator==(long int left, const jvar &right)
{
    return jvar(left) == right;
}
bool operator==(long long int left, const jvar &right)
{
    return jvar(left) == right;
}
bool operator==(const char left[], const jvar &right)
{
    return jvar(left) == right;
}



bool operator!=(jvar left, const double &right)
{
    return left != jvar(right);
}
bool operator!=(jvar left, const int &right)
{
    return left != jvar(right);
}
bool operator!=(jvar left, const long int &right)
{
    return left != jvar(right);
}
bool operator!=(jvar left, const long long int &right)
{
    return left != jvar(right);
}
bool operator!=(jvar left, const short int &right)
{
    return left != jvar(right);
}
bool operator!=(jvar left, const float &right)
{
    return left != jvar(right);
}
bool operator!=(jvar left, const char &right)
{
    return left != jvar(right);
}
bool operator!=(jvar left, const char right[])
{
    return left != jvar(right);
}
bool operator!=(double left, const jvar &right)
{
    return jvar(left) != right;
}
bool operator!=(float left, const jvar &right)
{
    return jvar(left) != right;
}
bool operator!=(char left, const jvar &right)
{
    return jvar(left) != right;
}
bool operator!=(short int left, const jvar &right)
{
    return jvar(left) != right;
}
bool operator!=(int left, const jvar &right)
{
    return jvar(left) != right;
}
bool operator!=(long int left, const jvar &right)
{
    return jvar(left) != right;
}
bool operator!=(long long int left, const jvar &right)
{
    return jvar(left) != right;
}
bool operator!=(const char left[], const jvar &right)
{
    return jvar(left) != right;
}



bool operator>(jvar left, const double &right)
{
    return left > jvar(right);
}
bool operator>(jvar left, const int &right)
{
    return left > jvar(right);
}
bool operator>(jvar left, const long int &right)
{
    return left > jvar(right);
}
bool operator>(jvar left, const long long int &right)
{
    return left > jvar(right);
}
bool operator>(jvar left, const short int &right)
{
    return left > jvar(right);
}
bool operator>(jvar left, const float &right)
{
    return left > jvar(right);
}
bool operator>(jvar left, const char &right)
{
    return left > jvar(right);
}
bool operator>(jvar left, const char right[])
{
    return left > jvar(right);
}
bool operator>(double left, const jvar &right)
{
    return jvar(left) > right;
}
bool operator>(float left, const jvar &right)
{
    return jvar(left) > right;
}
bool operator>(char left, const jvar &right)
{
    return jvar(left) > right;
}
bool operator>(short int left, const jvar &right)
{
    return jvar(left) > right;
}
bool operator>(int left, const jvar &right)
{
    return jvar(left) > right;
}
bool operator>(long int left, const jvar &right)
{
    return jvar(left) > right;
}
bool operator>(long long int left, const jvar &right)
{
    return jvar(left) > right;
}
bool operator>(const char left[], const jvar &right)
{
    return jvar(left) > right;
}




bool operator<(jvar left, const double &right)
{
    return left < jvar(right);
}
bool operator<(jvar left, const int &right)
{
    return left < jvar(right);
}
bool operator<(jvar left, const long int &right)
{
    return left < jvar(right);
}
bool operator<(jvar left, const long long int &right)
{
    return left < jvar(right);
}
bool operator<(jvar left, const short int &right)
{
    return left < jvar(right);
}
bool operator<(jvar left, const float &right)
{
    return left < jvar(right);
}
bool operator<(jvar left, const char &right)
{
    return left < jvar(right);
}
bool operator<(jvar left, const char right[])
{
    return left < jvar(right);
}
bool operator<(double left, const jvar &right)
{
    return jvar(left) < right;
}
bool operator<(float left, const jvar &right)
{
    return jvar(left) < right;
}
bool operator<(char left, const jvar &right)
{
    return jvar(left) < right;
}
bool operator<(short int left, const jvar &right)
{
    return jvar(left) < right;
}
bool operator<(int left, const jvar &right)
{
    return jvar(left) < right;
}
bool operator<(long int left, const jvar &right)
{
    return jvar(left) < right;
}
bool operator<(long long int left, const jvar &right)
{
    return jvar(left) < right;
}
bool operator<(const char left[], const jvar &right)
{
    return jvar(left) < right;
}



bool operator>=(jvar left, const double &right)
{
    return left >= jvar(right);
}
bool operator>=(jvar left, const int &right)
{
    return left >= jvar(right);
}
bool operator>=(jvar left, const long int &right)
{
    return left >= jvar(right);
}
bool operator>=(jvar left, const long long int &right)
{
    return left >= jvar(right);
}
bool operator>=(jvar left, const short int &right)
{
    return left >= jvar(right);
}
bool operator>=(jvar left, const float &right)
{
    return left >= jvar(right);
}
bool operator>=(jvar left, const char &right)
{
    return left >= jvar(right);
}
bool operator>=(jvar left, const char right[])
{
    return left >= jvar(right);
}
bool operator>=(double left, const jvar &right)
{
    return jvar(left) >= right;
}
bool operator>=(float left, const jvar &right)
{
    return jvar(left) >= right;
}
bool operator>=(char left, const jvar &right)
{
    return jvar(left) >= right;
}
bool operator>=(short int left, const jvar &right)
{
    return jvar(left) >= right;
}
bool operator>=(int left, const jvar &right)
{
    return jvar(left) >= right;
}
bool operator>=(long int left, const jvar &right)
{
    return jvar(left) >= right;
}
bool operator>=(long long int left, const jvar &right)
{
    return jvar(left) >= right;
}
bool operator>=(const char left[], const jvar &right)
{
    return jvar(left) >= right;
}




bool operator<=(jvar left, const double &right)
{
    return left <= jvar(right);
}
bool operator<=(jvar left, const int &right)
{
    return left <= jvar(right);
}
bool operator<=(jvar left, const long int &right)
{
    return left <= jvar(right);
}
bool operator<=(jvar left, const long long int &right)
{
    return left <= jvar(right);
}
bool operator<=(jvar left, const short int &right)
{
    return left <= jvar(right);
}
bool operator<=(jvar left, const float &right)
{
    return left <= jvar(right);
}
bool operator<=(jvar left, const char &right)
{
    return left <= jvar(right);
}
bool operator<=(jvar left, const char right[])
{
    return left <= jvar(right);
}
bool operator<=(double left, const jvar &right)
{
    return jvar(left) <= right;
}
bool operator<=(float left, const jvar &right)
{
    return jvar(left) <= right;
}
bool operator<=(char left, const jvar &right)
{
    return jvar(left) <= right;
}
bool operator<=(short int left, const jvar &right)
{
    return jvar(left) <= right;
}
bool operator<=(int left, const jvar &right)
{
    return jvar(left) <= right;
}
bool operator<=(long int left, const jvar &right)
{
    return jvar(left) <= right;
}
bool operator<=(long long int left, const jvar &right)
{
    return jvar(left) <= right;
}
bool operator<=(const char left[], const jvar &right)
{
    return jvar(left) <= right;
}


jvar& jvar::operator+=( const double &right)
{
    (*this) = (*this) + jvar(right);
    return (*this);
}
jvar& jvar::operator+=( const int &right)
{
    (*this) = (*this) + jvar(right);
    return (*this);
}
jvar& jvar::operator+=( const long int &right)
{
    (*this) = (*this) + jvar(right);
    return (*this);
}
jvar& jvar::operator+=( const long long int &right)
{
    (*this) = (*this) + jvar(right);
    return (*this);
}
jvar& jvar::operator+=( const short int &right)
{
    (*this) = (*this) + jvar(right);
    return (*this);
}
jvar& jvar::operator+=( const float &right)
{
    (*this) = (*this) + jvar(right);
    return (*this);
}
jvar& jvar::operator+=( const char &right)
{
    (*this) = (*this) + jvar(right);
    return (*this);
}
jvar& jvar::operator+=( const char right[])
{
    (*this) = (*this) + jvar(right);
    return (*this);
}
jvar& jvar::operator+=( const std::string right)
{
    (*this) = (*this) + jvar(right);
    return (*this);
}



jvar& jvar::operator-=( const double &right)
{
    (*this) = (*this) - jvar(right);
    return (*this);
}
jvar& jvar::operator-=( const int &right)
{
    (*this) = (*this) - jvar(right);
    return (*this);
}
jvar& jvar::operator-=( const long int &right)
{
    (*this) = (*this) - jvar(right);
    return (*this);
}
jvar& jvar::operator-=( const long long int &right)
{
    (*this) = (*this) - jvar(right);
    return (*this);
}
jvar& jvar::operator-=( const short int &right)
{
    (*this) = (*this) - jvar(right);
    return (*this);
}
jvar& jvar::operator-=( const float &right)
{
    (*this) = (*this) - jvar(right);
    return (*this);
}
jvar& jvar::operator-=( const char &right)
{
    (*this) = (*this) - jvar(right);
    return (*this);
}



jvar& jvar::operator*=( const double &right)
{
    (*this) = (*this) * jvar(right);
    return (*this);
}
jvar& jvar::operator*=( const int &right)
{
    (*this) = (*this) * jvar(right);
    return (*this);
}
jvar& jvar::operator*=( const long int &right)
{
    (*this) = (*this) * jvar(right);
    return (*this);
}
jvar& jvar::operator*=( const long long int &right)
{
    (*this) = (*this) * jvar(right);
    return (*this);
}
jvar& jvar::operator*=( const short int &right)
{
    (*this) = (*this) * jvar(right);
    return (*this);
}
jvar& jvar::operator*=( const float &right)
{
    (*this) = (*this) * jvar(right);
    return (*this);
}
jvar& jvar::operator*=( const char &right)
{
    (*this) = (*this) * jvar(right);
    return (*this);
}



jvar& jvar::operator/=( const double &right)
{
    (*this) = (*this) / jvar(right);
    return (*this);
}
jvar& jvar::operator/=( const int &right)
{
    (*this) = (*this) / jvar(right);
    return (*this);
}
jvar& jvar::operator/=( const long int &right)
{
    (*this) = (*this) / jvar(right);
    return (*this);
}
jvar& jvar::operator/=( const long long int &right)
{
    (*this) = (*this) / jvar(right);
    return (*this);
}
jvar& jvar::operator/=( const short int &right)
{
    (*this) = (*this) / jvar(right);
    return (*this);
}
jvar& jvar::operator/=( const float &right)
{
    (*this) = (*this) / jvar(right);
    return (*this);
}
jvar& jvar::operator/=( const char &right)
{
    (*this) = (*this) / jvar(right);
    return (*this);
}