//
//  MLutBool.m
//  ExportPanel
//
//  Created by denn on 11.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

#import "MLutBoolModel.h"

@implementation MLutBoolModel
@dynamic serial;
@dynamic datetime;

- (instancetype) initWith:(BOOL)state {
    self = [super init];
    if (self) {
        _nsstate = [NSNumber numberWithBool:state];
    }
    return self;
}

@end

@implementation MLutPublish
@dynamic nsstate;
+ (NSString*) name {
    return  @"mlutPublish";
}
@end

@implementation MLutPrinted
@dynamic nsstate;
+ (NSString*) name {
    return  @"mlutPrinted";
}
@end
