//
//  ServerProfileManager.swift
//  ShadowsocksX-NG
//
//  Created by 邱宇舟 on 16/6/6.
//  Copyright © 2016年 qiuyuzhou. All rights reserved.
//

import Cocoa

class AppProfileManager {
    
    static let instance:ServerProfileManager = ServerProfileManager()
    
    var profiles:[ServerProfile]
    var activeProfileId: String?
    
    fileprivate override init() {
        profiles = [ServerProfile]()
        
        let defaults = UserDefaults.standard
        if let _profiles = defaults.array(forKey: "ServerProfiles") {
            for _profile in _profiles {
                let profile = AppProfile.fromDictionary(_profile as! [String : AnyObject])
                profiles.append(profile)
            }
        }
        activeProfileId = defaults.string(forKey: "ActiveServerProfileId")
    }
    
    func setActiveProfiledId(_ id: String) {
        activeProfileId = id
        let defaults = UserDefaults.standard
        defaults.set(id, forKey: "ActiveServerProfileId")
    }
    
    func save() {
        let defaults = UserDefaults.standard
        var _profiles = [AnyObject]()
        for profile in profiles {
            if profile.isValid() {
                let _profile = profile.toDictionary()
                _profiles.append(_profile as AnyObject)
            }
        }
        defaults.set(_profiles, forKey: "ServerProfiles")
        
        // Deal with ActivedProfile
        if getActivedProfile() == nil {
            activeProfileId = nil
        }
        
        if activeProfileId != nil {
            defaults.set(activeProfileId, forKey: "ActiveServerProfileId")
            writeSSLocalConfFile((getActiveProfile()?.toJsonConfig())!)
        } else {
            defaults.removeObject(forKey: "ActiveServerProfileId")
            removeSSLocalConfFile()
        }
    }
    
    func getActivedProfile() -> AppProfile? {
        if let uuid = activeProfileId {
            for profile in profiles {
                if (profile.uuid == uuid) {
                    return profile
                }
            }
            return nil
        } else {
            return nil
        }
    }
}
