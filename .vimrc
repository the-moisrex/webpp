set tabstop=4
set softtabstop=4
set shiftwidth=4
set noexpandtab

set colorcolumn=80
highlight ColorColumn ctermbg=darkgray

augroup project
  autocmd!
  autocmd BufRead,BufNewFile *.h,*.cpp set filetype=cpp.doxygen
augroup END


let &path.="/usr/include/"
