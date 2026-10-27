#import "AppDelegate.h"
#import <DehancerCommon/License.h>

@interface AppDelegate ()

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
        // Insert code here to initialize your application

    LicenseType t = LicenseTypePro;
    NSLog(@" ... %@ ", @(t));
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
        // Insert code here to tear down your application
}

@end
