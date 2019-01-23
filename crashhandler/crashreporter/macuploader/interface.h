#ifndef __MYOBJC_INTERFACE_H__
#define __MYOBJC_INTERFACE_H__

class UploaderInterface
{
    public:
        UploaderInterface();
        ~UploaderInterface();

        void init(const char* url, const char* path, const char* name, const char* ver);
        void upload();
    private:
        void* self;
};

#endif 
