//
//  MLutExpandMode.swift
//  Dehancer mLut Maker
//
//  Created by denn on 03/12/2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

import Foundation
import IMProcessing

extension IMPBlendingMode {
    public init?(meta: ImageMeta) throws {
        let t = try meta.getField(MLutExpandModeModel.self, fieldId: nil) as! MLutExpandModeModel
        self.init(rawValue: t.nsmode.uint32Value)
    }
    
    public var model:MLutExpandModeModel {
        let m = MLutExpandModeModel()
        m.nsmode = NSNumber(value: self.rawValue)
        return m
    }
}
