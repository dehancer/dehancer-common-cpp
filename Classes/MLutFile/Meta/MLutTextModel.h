//
//  MLutTextModel.h
//  Dehancer mLut Maker
//
//  Created by denn on 30/09/2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <IMProcessingXMP/ImageMeta.h>

NS_ASSUME_NONNULL_BEGIN

@interface MLutTextModel : ImageMetaField
@property(nonnull)  NSString *nstext;
-(instancetype) initWith:(NSString*)text;
@end

@interface MLutName : MLutTextModel
@property(nonnull)  NSString *nstext;
@end

@interface MLutDescription : MLutTextModel
@property(nonnull)  NSString *nstext;
@end

NS_ASSUME_NONNULL_END
