#ifndef MENUSERVICE_H
#define MENUSERVICE_H

#include <Arduino.h>
#include <vector>
#include <functional>
#include "DisplayUtils.h"
#include "Device.h"

namespace NuggetsInc {

// Forward declaration
class MenuService;

// Menu item structure
struct MenuItem {
    String label;
    std::function<void()> action;
    bool enabled;
    
    MenuItem(const String& label, std::function<void()> action = nullptr, bool enabled = true)
        : label(label), action(action), enabled(enabled) {}
};

// Menu configuration
struct MenuConfig {
    String title;
    int startY;
    int lineHeight;
    uint8_t titleTextSize;
    uint8_t itemTextSize;
    uint16_t backgroundColor;
    uint16_t textColor;
    uint16_t selectedBackgroundColor;
    uint16_t selectedTextColor;
    uint16_t disabledTextColor;
    
    MenuConfig()
        : title("Menu"),
          startY(20),
          lineHeight(20),
          titleTextSize(2),
          itemTextSize(1),
          backgroundColor(COLOR_BLACK),
          textColor(COLOR_WHITE),
          selectedBackgroundColor(COLOR_WHITE),
          selectedTextColor(COLOR_BLACK),
          disabledTextColor(0x7BEF) // Gray
    {}
};

class MenuService {
public:
    MenuService(DisplayUtils* displayUtils = nullptr);
    ~MenuService();
    
    // Menu creation and management
    void createMenu(const String& title = "Menu");
    void addMenuItem(const String& label, std::function<void()> action = nullptr, bool enabled = true);
    void addMenuItem(const MenuItem& item);
    void insertMenuItem(int index, const String& label, std::function<void()> action = nullptr, bool enabled = true);
    void insertMenuItem(int index, const MenuItem& item);
    void removeMenuItem(int index);
    void clearMenu();
    
    // Menu item modification
    void setMenuItemLabel(int index, const String& label);
    void setMenuItemAction(int index, std::function<void()> action);
    void setMenuItemEnabled(int index, bool enabled);
    MenuItem* getMenuItem(int index);
    
    // Menu navigation
    void moveUp();
    void moveDown();
    void selectCurrent();
    void setSelectedIndex(int index);
    int getSelectedIndex() const;
    int getMenuItemCount() const;
    
    // Menu display
    void setConfig(const MenuConfig& config);
    MenuConfig& getConfig();
    void drawMenu();
    void drawMenu(const String& customTitle);
    void drawTitle();
    void drawTitle(const String& customTitle);
    void drawMenuItem(int index, bool isSelected);
    void redrawSelection();
    void clearScreen();
    
    // Display utilities
    void setDisplayUtils(DisplayUtils* displayUtils);
    DisplayUtils* getDisplayUtils() const;
    
    // Menu state
    bool isEmpty() const;
    bool isValidIndex(int index) const;
    
private:
    std::vector<MenuItem> menuItems_;
    int selectedIndex_;
    int previousSelectedIndex_;
    MenuConfig config_;
    DisplayUtils* displayUtils_;
    bool ownsDisplayUtils_;
    
    // Helper methods
    void ensureValidSelection();
    int calculateItemY(int index) const;
};

} // namespace NuggetsInc

#endif // MENUSERVICE_H
