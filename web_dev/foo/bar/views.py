from django.shortcuts import redirect, HttpResponse, render
from django.db import IntegrityError
from django.urls import reverse

from .models import User 

# Create your views here.
def index(request):
    return render(request, 'bar/index.html', {})

def register(request):
    if request.method == 'POST':
        username = request.POST['username']
        email = request.POST['email']
        password = request.POST['password']
        confirmation = request.POST['confirmation']
        if password != confirmation:
            return HttpResponse("error")

        try:
            user = User.objects.create_user(username, email, password)
            user.save()
        except IntegrityError:
            return render(request, "auctions/register.html", {
                "message": "Username already taken."
            })
        login(request, user)
        return redirect(reverse("index"))

    return render(request, 'bar/register.html', {})

def login(request):
    return render(request, 'bar/login.html', {})

