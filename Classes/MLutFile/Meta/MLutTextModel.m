//
//  MLutTextModel.m
//  Dehancer mLut Maker
//
//  Created by denn on 30/09/2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

#import "MLutTextModel.h"

@implementation MLutTextModel
@dynamic serial;
@dynamic datetime;

- (instancetype) initWith:(NSString*)text {
    self = [super init];
    if (self) {
        _nstext = text;
    }
    return self;
}
@end

@implementation MLutName
@dynamic nstext;
+ (NSString*) name {
    return  @"mlutName";
}
@end

@implementation MLutDescription
@dynamic nstext;
+ (NSString*) name {
    return  @"mlutdescription";
}
@end
