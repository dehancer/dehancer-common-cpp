//
//  MLutOptions.swift
//  Dehancer mLut Maker
//
//  Created by denn on 30/09/2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

import Foundation
import IMProcessingXMP
import IMProcessing

public struct MLutAttributes {
    
    public var type:MLutType           = .mlut
    public var lutSize:MLutSize        = .normal
    public var filmType:MLutFilmType   = .negative
    public var colorType:MLutColorType = .color
    public var name:MLutName?
    public var bundleDescription:MLutDescription?

    public var isPrinted:MLutPrinted = {
        let printed = MLutPrinted()
        printed.state = false
        return printed
    }()
    
    public var expandImpact:Float = 0
    public var expandMode:IMPBlendingMode = .normal

    @discardableResult public func store(url: URL, extension ext: String = "xmp") throws -> ImageMeta {
        let meta = ImageMeta(path: url.path, extension: ext, history:1)
        
        try meta.setField(type.model)
        try meta.setField(lutSize.model)
        try meta.setField(isPrinted)
        try meta.setField(filmType.model)
        try meta.setField(colorType.model)
        try meta.setField(expandImpact.model)
        try meta.setField(expandMode.model)

        if let name = name {
            try meta.setField(name)
        }
        if let desc = bundleDescription {
            try meta.setField(desc)
        }
        
        return meta
    }
    
    @discardableResult public mutating func restore(url: URL, extension ext: String = "xmp") throws -> ImageMeta {
        
        let meta = ImageMeta(path: url.path, extension: ext, history:1)
        
        guard let t = try MLutType(meta: meta) else {
            throw  NSError(domain: "com.dehancer.error",
                           code: Int(EINVAL),
                           userInfo: [
                            NSLocalizedDescriptionKey:
                                String(format: NSLocalizedString("Type of Multi Lut file is invalid", comment:"")),
                            NSLocalizedFailureReasonErrorKey:
                                String(format: NSLocalizedString("Open file error", comment:""))
                ])
        }
        type = t
        
        isPrinted = try MLutPrinted(meta: meta)
        lutSize = try MLutSize(meta: meta) ?? .normal
        filmType = try MLutFilmType(meta: meta) ?? .negative
        colorType = try MLutColorType(meta: meta) ?? .color
        name = try MLutName(meta: meta)
        bundleDescription = try MLutDescription(meta: meta)
        expandImpact = try Float(meta: meta) ?? 0
        expandMode = try IMPBlendingMode(meta: meta) ?? .normal

        return meta
    }
}
