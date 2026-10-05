#pragma once
#include <SFML/Graphics.hpp>

namespace Theme {
    // Background
    inline sf::Color windowBg()       { return sf::Color(235, 242, 252); }  // light blue-grey
    inline sf::Color sidebarBg()      { return sf::Color(13, 71, 161);   }  // deep blue
    inline sf::Color sidebarBtnActive(){ return sf::Color(21, 101, 192); }  // active blue
    inline sf::Color sidebarBtn()     { return sf::Color(13, 71, 161);   }
    inline sf::Color sidebarBtnHover(){ return sf::Color(25, 118, 210);  }

    // Header
    inline sf::Color headerBg()       { return sf::Color(255, 255, 255); }  // white header
    inline sf::Color headerBorder()   { return sf::Color(220, 230, 245); }
    inline sf::Color headerBg2()      { return sf::Color(26, 42, 74);    }  // navy (kept for compat)

    // Content
    inline sf::Color panelBg()        { return sf::Color(255, 255, 255); }
    inline sf::Color panelBorder()    { return sf::Color(220, 228, 240); }
    inline sf::Color contentBg()      { return sf::Color(245, 248, 255); }  // very light blue content

    // Buttons
    inline sf::Color primaryBtn()     { return sf::Color(25, 118, 210);  }  // blue login/save
    inline sf::Color primaryBtnHover(){ return sf::Color(13, 71, 161);   }
    inline sf::Color saveBtn()        { return sf::Color(46, 160, 67);   }  // green save
    inline sf::Color saveBtnHover()   { return sf::Color(30, 130, 50);   }
    inline sf::Color clearBtn()       { return sf::Color(120, 130, 145); }  // grey clear
    inline sf::Color deleteBtn()      { return sf::Color(192, 57, 43);   }

    // Text
    inline sf::Color textDark()       { return sf::Color(26, 42, 74);    }  // navy text
    inline sf::Color textMid()        { return sf::Color(90, 105, 130);  }
    inline sf::Color textLight()      { return sf::Color(140, 155, 175); }

    // Fields
    inline sf::Color textFieldBg()    { return sf::Color(255, 255, 255); }
    inline sf::Color textFieldBorder(){ return sf::Color(180, 200, 230); }
    inline sf::Color textFieldActive(){ return sf::Color(25, 118, 210);  }

    // Table
    inline sf::Color tableHeaderBg()  { return sf::Color(13, 71, 161);   }
    inline sf::Color tableRowA()      { return sf::Color(255, 255, 255); }
    inline sf::Color tableRowB()      { return sf::Color(232, 240, 253); }

    // Status
    inline sf::Color errorText()      { return sf::Color(192, 57, 43);   }
    inline sf::Color successText()    { return sf::Color(46, 160, 67);   }

    // Orange accent (Aditya brand)
    inline sf::Color brandOrange()    { return sf::Color(230, 110, 30);  }
    inline sf::Color brandNavy()      { return sf::Color(26, 42, 74);    }

    inline bool loadDefaultFont(sf::Font& font) {
        const char* candidates[] = {
            "C:\\Windows\\Fonts\\segoeui.ttf",
            "C:\\Windows\\Fonts\\arial.ttf",
            "C:\\Windows\\Fonts\\tahoma.ttf",
            "assets/font.ttf"
        };
        for (auto path : candidates) {
            if (font.loadFromFile(path)) return true;
        }
        return false;
    }
}
