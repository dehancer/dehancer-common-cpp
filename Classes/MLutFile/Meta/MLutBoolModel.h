//
//  MLutBool.h
//  ExportPanel
//
//  Created by denn on 11.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <IMProcessingXMP/ImageMeta.h>

@interface MLutBoolModel : ImageMetaField
@property(nonnull)  NSNumber *nsstate;
-(instancetype) initWith:(BOOL)state;
@end

@interface MLutPublish : MLutBoolModel
@property(nonnull)  NSNumber *nsstate;
@end

@interface MLutPrinted : MLutBoolModel
@property(nonnull)  NSNumber *nsstate;
@end
