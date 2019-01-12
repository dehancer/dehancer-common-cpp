//
//  MLutExpandImpact.swift
//  Dehancer mLut Maker
//
//  Created by denn on 03/12/2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

import Foundation

extension Float {
    public init?(meta: ImageMeta) throws {
        let t = try meta.getField(MLutExpandImpactModel.self, fieldId: nil) as! MLutExpandImpactModel
        self = t.nsvalue.floatValue
    }
    
    public var model:MLutExpandImpactModel {
        let m = MLutExpandImpactModel()
        m.nsvalue = NSNumber(value: self)
        return m
    }
}
