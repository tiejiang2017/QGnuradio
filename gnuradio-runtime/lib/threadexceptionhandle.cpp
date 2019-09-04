#include "threadexceptionhandle.h"

threadExceptionHandle::threadExceptionHandle()
{

}

void threadExceptionHandle::pushOne(std::string name, sigjmp_buf buf,uint threadID)
{
    boost::unique_lock<boost::mutex> lock(mutex);
    struct jmptagInfo info;
    info.jmptag = buf[0];
    info.addr = threadID;
    info.name = name;
    jmpMap[threadID] = info;
    flagMap[name] = 0;
    sbuf = buf[0];
}

void threadExceptionHandle::clearOwnFlag(std::string name)
{
    std::cout <<name<< " clearOwnFlag~+++~\n" ;
    boost::unique_lock<boost::mutex> lock(mutex);
    flagMap[name] = 0;
}
void threadExceptionHandle::setAllFlag()
{
    boost::unique_lock<boost::mutex> lock(mutex);
    for (const auto& n : flagMap) {
//            std::cout << "Key:[" << n.first << "] Value:[" << n.second << "]\n";
           flagMap[n.first] = 1;
    }
}

void threadExceptionHandle::setStackBuf(uint *buf)
{
    for(int i=0;i<1024;i++)
        errStackBuff[i] = buf[i];
}
__jmp_buf_tag threadExceptionHandle::getJmpbuf(uint id)
{
    boost::unique_lock<boost::mutex> lock(mutex);
    std::cout << "threadExceptionHandle::getJmpbuf~~~~~~~~~~~~~~~~~~~\n" ;
    return jmpMap[id].jmptag;
//    for (const auto& n : nameMap) {
//            if(n.second.addr == id)
//             {
//                std::cout << "return  ~~~~~~~~"<<n.first ;
//                return nameMap[n.first].jmptag;
//            }
//        }
//    std::cout << "return  sbufn" ;
    return sbuf;
}
