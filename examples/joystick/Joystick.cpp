
////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <sstream>
#include <iomanip>
#include <string>
#include <map>


namespace
{
    typedef std::map<std::string, std::pair<sf::Text, sf::Text> > Texts;
    Texts texts;
    std::ostringstream sstr;

    // Axes labels in as C strings
    const char* axislabels[] = {"X", "Y", "Z", "R", "U", "V", "PovX", "PovY"};

    // Helper to set text entries to a specified value
    template<typename T>
    void set(const char* label, const T& value)
    {
        sstr.str("");
        sstr << value;
        texts[label].second.setString(sstr.str());
    }

    // Update joystick identification
    void updateIdentification(unsigned int index)
    {
        sstr.str("");
        sstr << "Joystick " << index << ":";
        texts["ID"].first.setString(sstr.str());
        texts["ID"].second.setString(sf::Joystick::getIdentification(index).name);
    }

    // Update joystick axes
    void updateAxes(unsigned int index)
    {
        for (unsigned int j = 0; j < sf::Joystick::AxisCount; ++j)
        {
            if (sf::Joystick::hasAxis(index, static_cast<sf::Joystick::Axis>(j)))
                set(axislabels[j], sf::Joystick::getAxisPosition(index, static_cast<sf::Joystick::Axis>(j)));
        }
    }

    // Update joystick buttons
    void updateButtons(unsigned int index)
    {
        for (unsigned int j = 0; j < sf::Joystick::getButtonCount(index); ++j)
        {
            sstr.str("");
            sstr << "Button " << j;

            set(sstr.str().c_str(), sf::Joystick::isButtonPressed(index, j));
        }
    }

    // Helper to update displayed joystick values
    void updateValues(unsigned int index)
    {
        if (sf::Joystick::isConnected(index)) {
            // Update the label-value sf::Text objects based on the current joystick state
            updateIdentification(index);
            updateAxes(index);
            updateButtons(index);
        }
    }
}


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
int main()
{
    // Create the window of the application
    sf::RenderWindow window(sf::VideoMode(480, 640), "Joystick", sf::Style::Close);
    window.setVerticalSyncEnabled(true);

    // Load the text font
    sf::Font font;
    if (!font.loadFromFile("resources/sansation.ttf"))
        return EXIT_FAILURE;

    // Set up our string conversion parameters
    sstr.precision(2);
    sstr.setf(std::ios::fixed | std::ios::boolalpha);

    // Set up our label-value sf::Text objects
    texts["ID"].first.setPosition(5.f, 5.f);
    texts["ID"].second.setPosition(80.f, 5.f);

    for (unsigned int i = 0; i < sf::Joystick::AxisCount; ++i)
    {
        std::pair<sf::Text, sf::Text>& text = texts[axislabels[i]];

        text.first.setPosition(5.f, 5.f + ((i + 2) * font.getLineSpacing(14)));
        text.first.setString(std::string(axislabels[i]) + ":");

        text.second.setPosition(80.f, 5.f + ((i + 2) * font.getLineSpacing(14)));
    }

    for (unsigned int i = 0; i < sf::Joystick::ButtonCount; ++i)
    {
        sstr.str("");
        sstr << "Button " << i;
        std::pair<sf::Text, sf::Text>& text = texts[sstr.str()];

        text.first.setPosition(5.f, 5.f + ((sf::Joystick::AxisCount + i + 2) * font.getLineSpacing(14)));
        text.first.setString(sstr.str() + ":");

        text.second.setPosition(80.f, 5.f + ((sf::Joystick::AxisCount + i + 2) * font.getLineSpacing(14)));
    }

    for (Texts::iterator i = texts.begin(); i != texts.end(); ++i)
    {
        i->second.first.setFont(font);
        i->second.first.setCharacterSize(14);
        i->second.first.setFillColor(sf::Color::White);

        i->second.second.setFont(font);
        i->second.second.setCharacterSize(14);
        i->second.second.setFillColor(sf::Color::White);
    }

    while (window.isOpen())
    {
        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Window closed or escape key pressed: exit
            if ((event.type == sf::Event::Closed) ||
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                window.close();
                break;
            }
            else if ((event.type == sf::Event::JoystickButtonPressed) ||
                     (event.type == sf::Event::JoystickButtonReleased) ||
                     (event.type == sf::Event::JoystickMoved) ||
                     (event.type == sf::Event::JoystickConnected))
            {
                // Update displayed joystick values
                updateValues(event.joystickConnect.joystickId);
            }
            else if (event.type == sf::Event::JoystickDisconnected)
            {
                // Reset displayed joystick values to empty
                for (Texts::iterator i = texts.begin(); i != texts.end(); ++i)
                    i->second.second.setString("");

                texts["ID"].first.setString("");
            }
        }

        // Clear the window
        window.clear();

        // Draw the label-value sf::Text objects
        for (Texts::const_iterator i = texts.begin(); i != texts.end(); ++i)
        {
            window.draw(i->second.first);
            window.draw(i->second.second);
        }

        // Display things on screen
        window.display();
    }
}
