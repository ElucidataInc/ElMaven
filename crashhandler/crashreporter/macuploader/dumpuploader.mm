#import "dumpuploader.h"
#import "interface.h"
#import "common/mac/HTTPMultipartUpload.h"

@implementation DumpUploader

UploaderInterface::UploaderInterface ():
    self(NULL)
{}

UploaderInterface::~UploaderInterface()
{
    [(id)self dealloc];
}

void UploaderInterface::init(const char* url, 
                             const char* path,
                             const char* name, 
                             const char* ver)
{
    self = [DumpUploader alloc];

    DumpUploader* obj = (id)self;
    obj->url = [NSString stringWithUTF8String:url];
    obj->dumpfile = [NSString stringWithUTF8String:path];
    obj->prod = [NSString stringWithUTF8String:name];
    obj->ver = [NSString stringWithUTF8String:ver];

    NSLog(@"url: %@", obj->url);
    NSLog(@"path: %@", obj->dumpfile);
    NSLog(@"name: %@ ", obj->prod);
    NSLog(@"version: %@ ", obj->ver);

}

void UploaderInterface::upload()
{
    [(id)self upload];
}

- (void) upload
{
    NSURL *uploadUrl = [NSURL URLWithString:url];
    HTTPMultipartUpload *ul = [[HTTPMultipartUpload alloc] initWithURL:uploadUrl];
    NSMutableDictionary *parameters = [NSMutableDictionary dictionary];

    // Add parameters
    [parameters setObject:prod forKey:@"prod"];
    [parameters setObject:ver forKey:@"ver"];
    [ul setParameters:parameters];

    // Add file
    [ul addFileAtPath:dumpfile name:@"upload_file_minidump"];

    // Send it
    NSError *error = nil;
    NSData *data = [ul send:&error];
    NSString *result = [[NSString alloc] initWithData:data
                                       encoding:NSUTF8StringEncoding];

    NSLog(@"Send: %@", error ? [error description] : @"No Error");
    NSLog(@"Response: %ld", (long)[[ul response] statusCode]);
    NSLog(@"Result: %lu bytes\n%@", (unsigned long)[data length], result);

    [result release];
    [ul release];
}


@end

