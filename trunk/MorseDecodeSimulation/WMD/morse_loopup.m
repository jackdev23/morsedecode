function letters = morse_loopup(ditDahString)

spaces = strfind(ditDahString, '/');
letters='';
for i=1:length(spaces+1)  % number of characters
    %% new letter/number
    if(i==1)
        letters = strcat(letters,lookup_table(ditDahString(1:spaces(1)-1)));        
    else 
        letters = strcat(letters,lookup_table(ditDahString(spaces(i-1)+1:spaces(i)-1)));   
    end
    letters = strcat(letters,' ');
end

function letter = lookup_table(ditDah)

switch ditDah
    case '.-';      letter = 'A';
    case '-...';    letter = 'B';
    case '-.-.';    letter = 'C';
    case '-..';     letter = 'D';
    case '.';       letter = 'E';
    case '..-.';    letter = 'F';
    case '--.';     letter = 'G';
    case '....';    letter = 'H';
    case '..';      letter = 'I';
    case '.---';    letter = 'J';
    case '-.-';     letter = 'K';
    case '.-..';    letter = 'L';
    case '--';      letter = 'M';
    case '-.';      letter = 'N';
    case '---';     letter = 'O';
    case '.--.';    letter = 'P';
    case '--.-';    letter = 'Q';
    case '.-.';     letter = 'R';
    case '...';     letter = 'S';
    case '-';       letter = 'T';
    case '..-';     letter = 'U';
    case '...-';    letter = 'V';
    case '.--';     letter = 'W';
    case '-..-';    letter = 'X';
    case '-.--';    letter = 'Y';
    case '--..';    letter = 'Z';
    case '.----';    letter = '1';
    case '..---';   letter = '2';
    case '...--';   letter = '3';
    case '.---.';   letter = '4';
    case '.....';   letter = '5';
    case '-....';   letter = '6';
    case '--...';   letter = '7';    
    case '---..';   letter = '8';    
    case '----.';   letter = '9';    
    case '-----';   letter = '0';        
    otherwise       letter = '?';
end