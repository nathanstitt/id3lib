;;; $Id: emacs.el 1355 2005-11-22 15:34:52Z nas $

(defun my-save-and-compile ()
  (interactive "")
  (save-buffer 0)
  (compile "cd ~/code/ruby/id3lib/ext/id3lib && make && cd ../../test && ruby test_id3lib.rb"))


(defun my-save-and-compile ()
  (interactive "")
  (save-buffer 0)
  (compile "cd ~/code/ruby/id3lib/ext/id3lib && make"))

(defun my-save-and-compile ()
  (interactive "")
  (save-buffer 0)
  (compile "cd ~/code/ruby/id3lib/test && ruby test_load.rb"))


(defun my-save-and-compile ()
  (interactive "")
  (save-buffer 0)
  (compile "cd ~/code/ruby/id3lib && ./generate-html-docs"))

(defun my-save-and-compile ()
  (interactive "")
  (save-buffer 0)
  (compile "cd ~/code/ruby/id3lib && ruby setup.rb test"))


;; && ruby extconf.rb && make clean

;;; end of emacs.el
