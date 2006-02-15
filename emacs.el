;;; $Id: emacs.el 1355 2005-11-22 15:34:52Z nas $

(defun my-save-and-compile ()
  (interactive "")
  (save-buffer 0)
  (compile "cd ~/code/ruby/id3lib && make && cd test && ruby test.rb"))


(defun my-save-and-compile ()
  (interactive "")
  (save-buffer 0)
  (compile "cd ~/code/ruby/id3lib && ./generate-html-docs"))

;; && ruby extconf.rb && make clean

;;; end of emacs.el
