//
//  MLutTypeModel.h
//  ExportPanel
//
//  Created by denn on 09.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <IMProcessingXMP/ImageMeta.h>

typedef NS_ENUM(uint, MLutType) {
    MLutTypeMlut  = 0,
    MLutTypeCube  = 1,
    MLutTypePng   = 2
};

@interface MLutTypeModel : ImageMetaField
@property(nonnull)  NSNumber *nstype;
@end
