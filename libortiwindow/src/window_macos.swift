// window_macos.swift
import Cocoa

@_cdecl("swift_open_window")
public func swift_open_window() {
    let app = NSApplication.shared
    app.setActivationPolicy(.regular)

    let window = NSWindow(
        contentRect: NSRect(x: 100, y: 100, width: 800, height: 600),
        styleMask: [.titled, .closable, .resizable],
        backing: .buffered, defer: false)
    window.title = "My Swift Window"
    window.makeKeyAndOrderFront(nil)

    app.activate(ignoringOtherApps: true)
    app.run()
}
