//
//  MLutFilmType.h
//  ExportPanel
//
//  Created by denn on 11.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <IMProcessingXMP/ImageMeta.h>

typedef NS_ENUM(uint, MLutFilmType) {
    MLutFilmTypeNegative  = 0,
    MLutFilmTypePositive  = 1,
};

@interface MLutFilmTypeModel : ImageMetaField
@property(nonnull)  NSNumber *nstype;
@end
