//
//  MLutSize.swift
//  ExportPanel
//
//  Created by denn on 11.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

import Foundation
import IMProcessingXMP

public extension MLutSize {
    
    public init?(meta: ImageMeta) throws {
        let t = try meta.getField(MLutSizeModel.self, fieldId: nil) as! MLutSizeModel
        self.init(rawValue: t.nssize.uint32Value)
    }
    
    public var model:MLutSizeModel {
        let m = MLutSizeModel()
        m.nssize = NSNumber(value: self.rawValue)
        return m
    }
    
    public var size:Int {
        switch self {
        case .tiny:
            return 16
        case .small:
            return 32
        case .normal:
            return 64
        case .large:
            return 128
        case .huge:
            return 256
        }
    }
    
    public var caption:String {
        switch self {
        case .tiny:
            return NSLocalizedString("Tiny - \(size)x", comment: "")
        case .small:
            return NSLocalizedString("Small - \(size)x", comment: "")
        case .normal:
            return NSLocalizedString("Normal - \(size)x", comment: "")
        case .large:
            return NSLocalizedString("Large - \(size)x", comment: "")
        case .huge:
            return NSLocalizedString("Huge - \(size)x", comment: "")
        }
    }
    
    public static var availableList:[MLutSize] {
        return [.tiny, .small, .normal, .large, .huge]
    }
}
