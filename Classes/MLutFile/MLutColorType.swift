//
//  MLutColorType.swift
//  ExportPanel
//
//  Created by denn on 11.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

import Foundation

public extension MLutColorType {
    
    public init?(meta: ImageMeta) throws {
        let t = try meta.getField(MLutColorTypeModel.self, fieldId: nil) as! MLutColorTypeModel
        self.init(rawValue: t.nstype.uint32Value)
    }
    
    public var model:MLutColorTypeModel {
        let m = MLutColorTypeModel()
        m.nstype = NSNumber(value: self.rawValue)
        return m
    }
    
    public var caption:String {
        switch self {
        case .color:
            return NSLocalizedString("Color", comment: "")
        case .bw:
            return NSLocalizedString("BW", comment: "")
        default:
            return ""
        }
    }
    
    public static var availableList:[MLutColorType] {
        return [.color, .bw]
    }
}
