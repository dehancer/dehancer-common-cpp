//
//  MLutText.swift
//  Dehancer mLut Maker
//
//  Created by denn on 30/09/2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

import Foundation

public extension MLutTextModel {
    public var text:String {
        get { return nstext as String }
        set { nstext = newValue }
    }
}


public extension MLutName {
    
    public convenience init(meta: ImageMeta) throws {
        self.init()
        do {
            let t = try meta.getField(MLutName.self, fieldId: nil) as! MLutTextModel
            text = t.nstext as String
        }
        catch {
            text = ""
        }
    }

}

public extension MLutDescription {
    public convenience init(meta: ImageMeta) throws {
        self.init()
        do {
            let t = try meta.getField(MLutDescription.self, fieldId: nil) as! MLutTextModel
            text = t.nstext as String
        }
        catch {
            text = ""
        }
    }
}
