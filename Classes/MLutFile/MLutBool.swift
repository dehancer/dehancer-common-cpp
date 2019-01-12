//
//  MLutBool.swift
//  ExportPanel
//
//  Created by denn on 11.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

import Foundation
import IMProcessingXMP

public extension MLutBoolModel {
    public var state:Bool {
        get {
            return nsstate.boolValue
        }
        set {
            nsstate = NSNumber(value: newValue)
        }
    }
}

public extension MLutPrinted {
    public convenience init(meta: ImageMeta) throws {
        self.init()
        do {
            let t = try meta.getField(MLutPrinted.self, fieldId: nil) as! MLutBoolModel
            state = t.nsstate.boolValue
        }
        catch {
            state = false
        }
    }
}

public extension MLutPublish {
    public convenience init(meta: ImageMeta) throws {
        self.init()
        do {
            let t = try meta.getField(MLutPublish.self, fieldId: nil) as! MLutBoolModel
            state = t.nsstate.boolValue
        }
        catch {
            state = false
        }
    }
}
