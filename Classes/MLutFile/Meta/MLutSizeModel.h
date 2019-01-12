//
//  MLutSizeModel.h
//  ExportPanel
//
//  Created by denn on 11.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <IMProcessingXMP/ImageMeta.h>

typedef NS_ENUM(uint, MLutSize) {
    MLutSizeTiny   = 0,
    MLutSizeSmall  = 1,
    MLutSizeNormal = 2,
    MLutSizeLarge  = 3,
    MLutSizeHuge   = 4
};

@interface MLutSizeModel : ImageMetaField
@property(nonnull)  NSNumber *nssize;
@end
