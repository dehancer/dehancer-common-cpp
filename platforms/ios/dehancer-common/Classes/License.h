    //
    //  License.h
    //  DehancerCommon
    //
    //  Created by denn nevera on 08/10/2019.
    //

#import <Foundation/Foundation.h>

typedef NS_ENUM(int, LicenseType) {
    LicenseTypePromo = 0,
    LicenseTypeLight = 1,
    LicenseTypeStandard = 2,
    LicenseTypePro = 3,
    LicenseTypeUnknown = -1
};

NS_ASSUME_NONNULL_BEGIN

@interface License : NSObject

@end

NS_ASSUME_NONNULL_END
