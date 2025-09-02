#include "MenuService.h"
#include "Utils/Device.h"

namespace NuggetsInc {

MenuService::MenuService(DisplayUtils* displayUtils)
    : selectedIndex_(0),
      previousSelectedIndex_(-1),
      displayUtils_(displayUtils),
      ownsDisplayUtils_(false)
{
    if (!displayUtils_) {
        displayUtils_ = new DisplayUtils();
        ownsDisplayUtils_ = true;
    }
}

MenuService::~MenuService() {
    if (ownsDisplayUtils_ && displayUtils_) {
        delete displayUtils_;
    }
}

void MenuService::createMenu(const String& title) {
    clearMenu();
    config_.title = title;
    selectedIndex_ = 0;
    previousSelectedIndex_ = -1;
}

void MenuService::addMenuItem(const String& label, std::function<void()> action, bool enabled) {
    menuItems_.emplace_back(label, action, enabled);
    ensureValidSelection();
}

void MenuService::addMenuItem(const MenuItem& item) {
    menuItems_.push_back(item);
    ensureValidSelection();
}

void MenuService::insertMenuItem(int index, const String& label, std::function<void()> action, bool enabled) {
    if (index < 0 || index > static_cast<int>(menuItems_.size())) {
        index = menuItems_.size();
    }
    menuItems_.insert(menuItems_.begin() + index, MenuItem(label, action, enabled));
    ensureValidSelection();
}

void MenuService::insertMenuItem(int index, const MenuItem& item) {
    if (index < 0 || index > static_cast<int>(menuItems_.size())) {
        index = menuItems_.size();
    }
    menuItems_.insert(menuItems_.begin() + index, item);
    ensureValidSelection();
}

void MenuService::removeMenuItem(int index) {
    if (isValidIndex(index)) {
        menuItems_.erase(menuItems_.begin() + index);
        ensureValidSelection();
    }
}

void MenuService::clearMenu() {
    menuItems_.clear();
    selectedIndex_ = 0;
    previousSelectedIndex_ = -1;
}

void MenuService::setMenuItemLabel(int index, const String& label) {
    if (isValidIndex(index)) {
        menuItems_[index].label = label;
    }
}

void MenuService::setMenuItemAction(int index, std::function<void()> action) {
    if (isValidIndex(index)) {
        menuItems_[index].action = action;
    }
}

void MenuService::setMenuItemEnabled(int index, bool enabled) {
    if (isValidIndex(index)) {
        menuItems_[index].enabled = enabled;
    }
}

MenuItem* MenuService::getMenuItem(int index) {
    if (isValidIndex(index)) {
        return &menuItems_[index];
    }
    return nullptr;
}

void MenuService::moveUp() {
    if (menuItems_.empty()) return;
    
    previousSelectedIndex_ = selectedIndex_;
    selectedIndex_ = (selectedIndex_ - 1 + menuItems_.size()) % menuItems_.size();
    redrawSelection();
}

void MenuService::moveDown() {
    if (menuItems_.empty()) return;
    
    previousSelectedIndex_ = selectedIndex_;
    selectedIndex_ = (selectedIndex_ + 1) % menuItems_.size();
    redrawSelection();
}

void MenuService::selectCurrent() {
    if (isValidIndex(selectedIndex_) && menuItems_[selectedIndex_].enabled) {
        if (menuItems_[selectedIndex_].action) {
            menuItems_[selectedIndex_].action();
        }
    }
}

void MenuService::setSelectedIndex(int index) {
    if (isValidIndex(index)) {
        previousSelectedIndex_ = selectedIndex_;
        selectedIndex_ = index;
        redrawSelection();
    }
}

int MenuService::getSelectedIndex() const {
    return selectedIndex_;
}

int MenuService::getMenuItemCount() const {
    return menuItems_.size();
}

void MenuService::setConfig(const MenuConfig& config) {
    config_ = config;
}

MenuConfig& MenuService::getConfig() {
    return config_;
}

void MenuService::drawMenu() {
    if (!displayUtils_) return;

    clearScreen();
    drawTitle();

    for (int i = 0; i < static_cast<int>(menuItems_.size()); i++) {
        drawMenuItem(i, i == selectedIndex_);
    }
}

void MenuService::drawMenu(const String& customTitle) {
    if (!displayUtils_) return;

    clearScreen();
    drawTitle(customTitle);

    for (int i = 0; i < static_cast<int>(menuItems_.size()); i++) {
        drawMenuItem(i, i == selectedIndex_);
    }
}

void MenuService::drawTitle() {
    if (!displayUtils_) return;

    displayUtils_->setTextColor(config_.textColor);
    displayUtils_->setTextSize(config_.titleTextSize);
    displayUtils_->setCursor(0, 0);
    displayUtils_->println(config_.title);
}

void MenuService::drawTitle(const String& customTitle) {
    if (!displayUtils_) return;

    displayUtils_->setTextColor(config_.textColor);
    displayUtils_->setTextSize(config_.titleTextSize);
    displayUtils_->setCursor(0, 0);
    displayUtils_->println(customTitle);
}

void MenuService::drawMenuItem(int index, bool isSelected) {
    if (!displayUtils_ || !isValidIndex(index)) return;
    
    const MenuItem& item = menuItems_[index];
    int y = calculateItemY(index);
    
    // Draw background
    if (isSelected) {
        displayUtils_->fillRect(0, y - 2, SCREEN_WIDTH, config_.lineHeight - 2, config_.selectedBackgroundColor);
        displayUtils_->setTextColor(config_.selectedTextColor);
        displayUtils_->setTextSize(config_.titleTextSize);
    } else {
        displayUtils_->fillRect(0, y - 2, SCREEN_WIDTH, config_.lineHeight - 2, config_.backgroundColor);
        if (item.enabled) {
            displayUtils_->setTextColor(config_.textColor);
        } else {
            displayUtils_->setTextColor(config_.disabledTextColor);
        }
        displayUtils_->setTextSize(config_.itemTextSize);
    }
    
    // Draw text
    displayUtils_->setCursor(0, y);
    displayUtils_->println(item.label);
    
    // Reset text color
    displayUtils_->setTextColor(config_.textColor);
}

void MenuService::redrawSelection() {
    if (previousSelectedIndex_ != -1 && isValidIndex(previousSelectedIndex_)) {
        drawMenuItem(previousSelectedIndex_, false);
    }
    
    if (isValidIndex(selectedIndex_)) {
        drawMenuItem(selectedIndex_, true);
    }
}

void MenuService::clearScreen() {
    if (displayUtils_) {
        displayUtils_->fillScreen(config_.backgroundColor);
    }
}

void MenuService::setDisplayUtils(DisplayUtils* displayUtils) {
    if (ownsDisplayUtils_ && displayUtils_) {
        delete displayUtils_;
        ownsDisplayUtils_ = false;
    }
    displayUtils_ = displayUtils;
}

DisplayUtils* MenuService::getDisplayUtils() const {
    return displayUtils_;
}

bool MenuService::isEmpty() const {
    return menuItems_.empty();
}

bool MenuService::isValidIndex(int index) const {
    return index >= 0 && index < static_cast<int>(menuItems_.size());
}

void MenuService::ensureValidSelection() {
    if (menuItems_.empty()) {
        selectedIndex_ = 0;
    } else if (selectedIndex_ >= static_cast<int>(menuItems_.size())) {
        selectedIndex_ = menuItems_.size() - 1;
    }
}

int MenuService::calculateItemY(int index) const {
    return config_.startY + (index * config_.lineHeight);
}

} // namespace NuggetsInc
