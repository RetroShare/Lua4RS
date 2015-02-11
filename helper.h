#ifndef HELPER
#define HELPER

static void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

static std::string& stripHTMLTags(std::string& s)
{
    // Remove all special HTML characters
    bool done = false;
    while(!done)
    {
        // Look for start of tag:
        size_t leftPos = s.find('<');
        if(leftPos != std::string::npos)
        {
            // See if tag close is in this line:
            size_t rightPos = s.find('>', leftPos);
            if(rightPos == std::string::npos)
            {
                done = true;
                s.erase(leftPos);
            }
            else
                s.erase(leftPos, rightPos - leftPos + 1);
        }
        else
            done = true;
    }

    replaceAll(s, "&lt;", "<");
    replaceAll(s, "&gt;", ">");
    replaceAll(s, "&amp;", "&");
    replaceAll(s, "&nbsp;", " ");

    return s;
}

#endif // HELPER

