//
//  MLutFile.swift
//  ExportPanel
//
//  Created by denn on 09.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

import Foundation
import IMProcessing
import CryptoSwift
import Darwin
import IMProcessingXMP

public class MLutFile {
    
    static public var key:[UInt8] = []
    
    public typealias Attributes = MLutAttributes
        
    public var attributes = MLutAttributes()
    
    public var cLuts:[MLutExposureMode:IMPCLut] = [MLutExposureMode.under:cLutIdentity(),
                                               MLutExposureMode.normal:cLutIdentity(),
                                               MLutExposureMode.over:cLutIdentity()]
    
    static public func restore(url:URL) throws -> MLutFile? {
        let manager = FileManager()
        if manager.fileExists(atPath: url.path) {
            if url.pathExtension == MLutType.mlut.extention {
                let file = MLutFile(url: url, type: .mlut)
                try file.restore()
                return file
            }
            let f = NSLocalizedString("File %@ is not mlut format, could not be imported", comment:"")
            throw NSError(domain: "com.dehancer.error",
                          code: Int(EACCES),
                          userInfo: [
                            NSLocalizedDescriptionKey:
                                String(format: String.localizedStringWithFormat(f, url.path)),
                            NSLocalizedFailureReasonErrorKey:
                                String(format: NSLocalizedString("File error", comment:""))
                ])
        }
        let f = NSLocalizedString("File %@ not found", comment:"")
        throw NSError(domain: "com.dehancer.error",
                      code: Int(EACCES),
                      userInfo: [
                        NSLocalizedDescriptionKey:
                            String(format: String.localizedStringWithFormat(f, url.path)),
                        NSLocalizedFailureReasonErrorKey:
                            String(format: NSLocalizedString("File error", comment:""))
            ])
    }
    
    static public func restore(path:String) throws -> MLutFile? {
        return try restore(url: URL(fileURLWithPath: path))
    }
    
    static public func new(url:URL, type:MLutType = .mlut) -> MLutFile {
        return MLutFile(url: url, type: type)
    }
    
    static public func new(path:String) -> MLutFile {
        return new(url: URL(fileURLWithPath: path))
    }
    
    let cipher = try! Blowfish(key: MLutFile.key, padding: .pkcs7)

    @discardableResult public func commit() throws -> MLutFile {
        switch attributes.type {
        case .cube:
            try saveAsFolderWithCube()
        case .mlut:
            try saveAsmLut()
        case .png:
            try saveAsFolderWithPng()
       }
        return self
    }
    
    private func saveAsmLut() throws {
        
        let meta = try attributes.store(url: url, extension: MLutType.mlut.extention)

        let model = MLutModel()
        
        for l in cLuts {
            
            let _2d = try l.value.convert(to: .lut_2d, lutSize: attributes.lutSize.size)
            
            if let data = _2d.representation(using: .png) {
                let enc = try cipher.encrypt(data.bytes)
                model.clutList.append(enc.toBase64()!)
            }
            else {
                model.clutList.append("".bytes.toBase64() ?? "")
            }
            
            try meta.setField(model)
        }
    }
    
    private func checkFolder(suffix:String) throws -> URL {
        let manager = FileManager()
        var isDir : ObjCBool = false
        
        let _url = URL(fileURLWithPath: self.url.deletingPathExtension().path+"\(suffix)")

        if manager.fileExists(atPath: _url.path, isDirectory: &isDir) {
            if !isDir.boolValue {
                let f = NSLocalizedString("File %@ is a regular file", comment:"")
                throw NSError(domain: "com.dehancer.error",
                              code: Int(EACCES),
                              userInfo: [
                                NSLocalizedDescriptionKey:
                                    String(format: String.localizedStringWithFormat(f, url.path)),
                                NSLocalizedFailureReasonErrorKey:
                                    String(format: NSLocalizedString("File error", comment:""))
                    ])
            }
        }
        else {
            try manager.createDirectory(atPath: _url.path, withIntermediateDirectories: true, attributes: nil)
        }
        
        return _url
    }
    
    private func saveAsFolderWithPng() throws {
        let _url = try checkFolder(suffix: "")
        for l in cLuts {
            var _url = _url
            let _2d = try l.value.convert(to: .lut_2d, lutSize: attributes.lutSize.size)
            _url.appendPathComponent(l.key.name)
            _url.appendPathExtension(MLutType.png.extention)
            try _2d.write(to: _url.path, using: .png)
        }
    }
    
    private func saveAsFolderWithCube() throws {
        let _url = try checkFolder(suffix: "")
        for l in cLuts {
            var _url = _url
            _url.appendPathComponent(l.key.name)
            _url.appendPathExtension(MLutType.cube.extention)
            let _3d = try l.value.convert(to: .lut_3d, lutSize: attributes.lutSize.size)
            try _3d.write(cube: _url.path)
        }
    }
    
    private init(url:URL, type:MLutType) {
        self.url = url
        attributes.type = type
    }
    
    @discardableResult private func restore() throws -> MLutFile {
        
        let meta = try attributes.restore(url: url, extension: MLutType.mlut.extention)
        
        switch attributes.type {
        case .mlut:
            break
        default:
            throw  NSError(domain: "com.dehancer.error",
                           code: Int(EINVAL),
                           userInfo: [
                            NSLocalizedDescriptionKey:
                                String(format: NSLocalizedString("Type of Multi Lut file is not supported", comment:"")),
                            NSLocalizedFailureReasonErrorKey:
                                String(format: NSLocalizedString("Open file error", comment:""))
                ])
        }

        let model = try meta.getField(MLutModel.self, fieldId: nil) as! MLutModel
        
        for (k,l) in model.clutList.enumerated() {
            let data = Data(bytes: try cipher.decrypt(Array(base64: (l as! String))))
            let image = NSImage(data: data)!
            let mode = MLutExposureMode(index: k)!
            let lut = try IMPCLut(context: MLutFile.context, haldImage: image)
            cLuts[mode] = try lut.convert(to: .lut_3d)
        }
        
        return self
    }
    
    fileprivate var url:URL
    fileprivate static var context = IMPContext()
    
    fileprivate static func cLutIdentity() -> IMPCLut {
        return try! IMPCLut(context: context,
                            lutType: .lut_3d,
                            lutSize: MLutSize.large.size,
                            format: .float,
                            title: "Dehancer mLut Maker CLUT Source")
    }
}
