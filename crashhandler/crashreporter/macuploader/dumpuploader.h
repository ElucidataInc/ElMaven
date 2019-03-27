#import <Foundation/Foundation.h>

@interface DumpUploader : NSObject
{
    NSString* url;
    NSString* dumpfile;
    NSString* prod;
    NSString* ver;
}

- (void) upload;
@end
