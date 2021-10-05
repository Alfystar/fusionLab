function tf2latex(tf, name)

[num,den]=tfdata(tf);
syms s
d1 = digits; % records accuracy
fmt = format('short'); % records and sets formatStyle
digits(2); % sets accuracy

latexText = latex(vpa(poly2sym(cell2mat(num),s)/poly2sym(cell2mat(den),s)));
fprintf("$%s(s) = %s$\n",name,latexText);
format(fmt); % restor formatStyle
digits(d1); % restor accuracy

end