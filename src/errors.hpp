/*
 * This file is part of libe2ee.
 *
 * libe2ee is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libe2ee is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libe2ee.  If not, see <http://www.gnu.org/licenses/lgpl>.
 */

#ifndef afgh_errors_hpp
#define afgh_errors_hpp

#include <cstdio>
#include <cstdarg>
#include <stdexcept>

class AfghError : public std::runtime_error{
public:
    AfghError(const char *file, int line, const char* fmt, ...) : std::runtime_error ("") {
        char buffer[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buffer, sizeof(buffer)-1, fmt, args);
        va_end(args);
        
        snprintf(message, sizeof(message)-1, "In %s(%d): %s", file, line, buffer);
    }
    
    const char* what() const throw() { return &message[0]; }
    
private:
    char message[1024];
};


#define afgh_throw_line(args...) throw AfghError( __FILE__, __LINE__, args)

#endif /* afgh_errors_hpp */
