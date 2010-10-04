function letter_sequence = getMorseLetterSequence(letter,dit,ssp,dah)

switch (letter)
  case 'A', letter_sequence = [dit;ssp;dah];                        
  case 'B', letter_sequence= [dah;ssp;dit;ssp;dit;ssp;dit];        
  case 'C', letter_sequence= [dah;ssp;dit;ssp;dah;ssp;dit];        
  case 'D', letter_sequence= [dah;ssp;dit;ssp;dit];                
  case 'E', letter_sequence= [dit];                                
  case 'F', letter_sequence= [dit;ssp;dit;ssp;dah;ssp;dit];        
  case 'G', letter_sequence= [dah;ssp;dah;ssp;dit];                
  case 'H', letter_sequence= [dit;ssp;dit;ssp;dit;ssp;dit];        
  case 'I', letter_sequence= [dit;ssp;dit];                        
  case 'J', letter_sequence= [dit;ssp;dah;ssp;dah;ssp;dah];        
  case 'K', letter_sequence= [dah;ssp;dit;ssp;dah];                
  case 'L', letter_sequence= [dit;ssp;dah;ssp;dit;ssp;dit];        
  case 'M', letter_sequence= [dah;ssp;dah];                        
  case 'N', letter_sequence= [dah;ssp;dit];                        
  case 'O', letter_sequence= [dah;ssp;dah;ssp;dah];                
  case 'P', letter_sequence= [dit;ssp;dah;ssp;dah;ssp;dit];        
  case 'Q', letter_sequence= [dah;ssp;dah;ssp;dit;ssp;dah];        
  case 'R', letter_sequence= [dit;ssp;dah;ssp;dit];                
  case 'S', letter_sequence= [dit;ssp;dit;ssp;dit];                
  case 'T', letter_sequence= dah;                                
  case 'U', letter_sequence= [dit;ssp;dit;ssp;dah];                
  case 'V', letter_sequence= [dit;ssp;dit;ssp;dit;ssp;dah];        
  case 'W', letter_sequence= [dit;ssp;dah;ssp;dah];                
  case 'X', letter_sequence= [dah;ssp;dit;ssp;dit;ssp;dah];        
  case 'Y', letter_sequence= [dah;ssp;dit;ssp;dah;ssp;dah];        
  case 'Z', letter_sequence= [dah;ssp;dah;ssp;dit;ssp;dit];        
  case '1', letter_sequence= [dit;ssp;dah;ssp;dah;ssp;dah;ssp;dah];
  case '2', letter_sequence= [dit;ssp;dit;ssp;dah;ssp;dah;ssp;dah];
  case '3', letter_sequence= [dit;ssp;dit;ssp;dit;ssp;dah;ssp;dah];
  case '4', letter_sequence= [dit;ssp;dit;ssp;dit;ssp;dit;ssp;dah];
  case '5', letter_sequence= [dit;ssp;dit;ssp;dit;ssp;dit;ssp;dit];
  case '6', letter_sequence= [dah;ssp;dit;ssp;dit;ssp;dit;ssp;dit];
  case '7', letter_sequence= [dah;ssp;dah;ssp;dit;ssp;dit;ssp;dit];
  case '8', letter_sequence= [dah;ssp;dah;ssp;dah;ssp;dit;ssp;dit];
  case '9', letter_sequence= [dah;ssp;dah;ssp;dah;ssp;dah;ssp;dit];
  case '0', letter_sequence= [dah;ssp;dah;ssp;dah;ssp;dah;ssp;dah];  
    otherwise, letter_sequence = NaN;
end





































