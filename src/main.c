/*
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "lisp.h"

#include <stdio.h>
#include <errno.h>

int main(int argc, char **argv) {
  FILE *read_from;
  
  switch (argc) {
  case 1:
    read_from = NULL;
    break;
  case 2: {
    read_from = fopen(argv[1], "r");
    if (!read_from) {
      perror("Error while opening main file: ");
    }
    break;
  }
  default:
    printf("Too many arguments. Use like %s [file]", argv[0]);
    break;
  }
  
  lisp_initialize();
  while (C_TRUE) {
    printf(">>> ");
    c_print(c_eval(c_read(read_from)));
  }
}
