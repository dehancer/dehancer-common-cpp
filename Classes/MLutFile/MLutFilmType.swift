//
//  MLutFilmType.swift
//  ExportPanel
//
//  Created by denn on 11.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

import Foundation

public extension MLutFilmType {
    
    public init?(meta: ImageMeta) throws {
        let t = try meta.getField(MLutFilmTypeModel.self, fieldId: nil) as! MLutFilmTypeModel
        self.init(rawValue: t.nstype.uint32Value)
    }
    
    public var model:MLutFilmTypeModel {
        let m = MLutFilmTypeModel()
        m.nstype = NSNumber(value: self.rawValue)
        return m
    }
    
    public var caption:String {
        switch self {
        case .positive:
            return NSLocalizedString("Positive Film Scan Target", comment: "")
        case .negative:
            return NSLocalizedString("Negative Film Scan Tagret", comment: "")
      }
    }
    
    public static var availableList:[MLutFilmType] {
        return [.negative, .positive]
    }
}
