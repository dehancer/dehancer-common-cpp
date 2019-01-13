//
//  MLutColorType.h
//  ExportPanel
//
//  Created by denn on 11.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <IMProcessingXMP/ImageMeta.h>

typedef NS_SWIFT_BRIDGED_TYPEDEF NS_ENUM(uint, MLutColorType) {
    MLutColorTypeColor = 0,
    MLutColorTypeBw    = 1,
};

//typedef uint MLutColorType NS_TYPED_EXTENSIBLE_ENUM;
//MLutColorType const MLutColorTypeColor = 0;
//MLutColorType const MLutColorTypeBw = 1;

@interface MLutColorTypeModel : ImageMetaField
@property(nonnull)  NSNumber *nstype;
@end
